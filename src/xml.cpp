#include "xml.hpp"
#include "exception.hpp"
#include <libxml/xpath.h>

namespace pparam
{
namespace xml
{

xml::Node *_convert_node(xmlNode *_node)
{
    xml::Node *res = nullptr;
    if (_node) {
        xml::Node::create_wrapper(_node);
        res = static_cast<xml::Node *>(_node->_private);
    }
    return res;
}

template <typename Tlist> Tlist get_children_common(const std::string &name, xmlNode *child)
{
    Tlist children;
    while (child) {
        if (name.empty() || name == (const char *)child->name)
            children.push_back(_convert_node(child));
        child = child->next;
    }
    return children;
}

/* Implementation of "Node" class */

Node::Node(xmlNode *_node) : node(_node)
{
    if (!node)
        throw Exception("xmlNode pointer cannot be nullptr.", TracePoint("xml"));
    node->_private = this;
}

xmlNode *Node::get_node() const { return node; }

std::string Node::get_name() const
{
    return node->name ? std::string((const char *)node->name) : std::string("");
}

Node::NodeList Node::get_children(const std::string &name)
{
    return get_children_common<NodeList>(name, node->children);
}

Node::const_NodeList Node::get_children(const std::string &name) const
{
    return get_children_common<const_NodeList>(name, node->children);
}

void Node::create_wrapper(xmlNode *_node)
{
    if (_node->_private)
        return;
    switch (_node->type) {
    case XML_ELEMENT_NODE:
        _node->_private = new Element(_node);
        break;
    case XML_COMMENT_NODE:
        _node->_private = new CommentNode(_node);
        break;
    case XML_CDATA_SECTION_NODE:
        _node->_private = new CDataNode(_node);
        break;
    case XML_TEXT_NODE:
        _node->_private = new TextNode(_node);
        break;
    default:
        break;
    }
}

void Node::free_wrappers(xmlNode *_node)
{
    if (!_node)
        return;
    if (_node->type != XML_ENTITY_REF_NODE) {
        for (auto child = _node->children; child; child = child->next)
            free_wrappers(child);
    }

    switch (_node->type) {
    case XML_ELEMENT_NODE:
    case XML_TEXT_NODE:
    case XML_COMMENT_NODE:
    case XML_CDATA_SECTION_NODE: {
        delete static_cast<Node *>(_node->_private);
        _node->_private = nullptr;
        break;
    }
    case XML_DOCUMENT_NODE:
        return;
    default:
        break;
    }
}

Node::~Node() {}

/* Implementation of "Element" class */

Element::Element(xmlNode *_node) : Node(_node) {}

std::string Element::get_attribute(const std::string &key) const
{
    const xmlChar *const value = xmlGetProp(get_node(), (const xmlChar *)key.c_str());
    return value ? std::string((const char *)value) : std::string("");
}

Element::~Element() {}

/* Implementation of "ContentNode" class */

ContentNode::ContentNode(xmlNode *_node) : Node(_node) {}

std::string ContentNode::get_content() const
{
    return get_node()->content ? std::string((const char *)get_node()->content) : std::string("");
}

ContentNode::~ContentNode() {}

/* Implementation of "CommentNode" class */

CommentNode::CommentNode(xmlNode *_node) : ContentNode(_node) {}

CommentNode::~CommentNode() {}

/* Implementation of "CDataNode" class */

CDataNode::CDataNode(xmlNode *_node) : ContentNode(_node) {}

CDataNode::~CDataNode() {}

/* Implementation of "TextNode" class */

TextNode::TextNode(xmlNode *_node) : ContentNode(_node) {}

TextNode::~TextNode() {}

/* Implementation of "Document" class */

Document::Document() : document(nullptr) {}

Element *Document::get_root_node()
{
    xmlNode *root = xmlDocGetRootElement(document);
    if (root == NULL) {
        throw Exception("no root node.", TracePoint("xml"));
    }
    Node::create_wrapper(root);
    return reinterpret_cast<Element *>(root->_private);
}

void Document::parseXmlStr(const std::string &xmlStr)
{
    if (document != NULL)
        xmlFreeDoc(document);
    document = xmlParseDoc((const xmlChar *)xmlStr.c_str());
    if (document == NULL) {
        xmlErrorPtr error = xmlGetLastError();
        if (error)
            throw Exception(("Can't parse document : " + std::string(error->message)),
                            TracePoint("xml"));
        else
            throw Exception("Can't parse document : xml is empty.", TracePoint("xml"));
    }
    document->_private = this;
}

void Document::parseXmlFile(const std::string &filePath)
{
    if (document != NULL)
        xmlFreeDoc(document);
    document = xmlReadFile(filePath.c_str(), NULL, 0);
    if (document == NULL) {
        xmlErrorPtr error = xmlGetLastError();
        if (error)
            throw Exception(("Can't parse document : " + std::string(error->message)),
                            TracePoint("xml"));
        else
            throw Exception("Can't parse document : xml is empty.", TracePoint("xml"));
    }
    document->_private = this;
}

std::string Document::toString() const
{
    xmlChar *_xmlChar;
    int size;
    xmlDocDumpMemory(document, &_xmlChar, &size);
    std::string xmlStr = (char *)_xmlChar;
    xmlFree(_xmlChar);
    return xmlStr;
}

std::string Document::queryXml(const std::string &query, const std::string &partialTag)
{
    std::string errorStr = "unknown error";
    xmlXPathContextPtr context;
    xmlXPathObjectPtr result;

    context = xmlXPathNewContext(document);
    if (context == NULL) {
        xmlErrorPtr error = xmlGetLastError();
        if (error)
            errorStr = error->message;
        throw Exception(("Can't execute query : " + errorStr), TracePoint("xml"));
    }

    result = xmlXPathEvalExpression(((const xmlChar *)query.c_str()), context);
    xmlXPathFreeContext(context);

    if (result == NULL) {
        xmlErrorPtr error = xmlGetLastError();
        if (error)
            errorStr = error->message;
        throw Exception(("Can't execute query : " + errorStr), TracePoint("xml"));
    }

    if (xmlXPathNodeSetIsEmpty(result->nodesetval)) {
        xmlXPathFreeObject(result);
        return "";
    }

    std::string data = "";
    xmlBufferPtr buffer = xmlBufferCreate();
    for (int i = 0; i < result->nodesetval->nodeNr; ++i) {
        if (xmlNodeDump(buffer, document, result->nodesetval->nodeTab[i], 0, 0) == -1) {
            xmlBufferFree(buffer);
            xmlXPathFreeObject(result);
            xmlErrorPtr error = xmlGetLastError();
            if (error)
                errorStr = error->message;
            throw Exception(("Can't execute query : " + errorStr), TracePoint("xml"));
        } else if (!partialTag.empty()) {
            data += "<" + partialTag + ">" + ((char *)buffer->content) + "</" + partialTag + ">";
            xmlBufferEmpty(buffer);
        }
    }

    if (partialTag.empty()) {
        data = ((char *)buffer->content);
    }

    xmlBufferFree(buffer);
    xmlXPathFreeObject(result);

    return data;
}

xmlDoc *Document::get_document() const { return document; }

Document::~Document()
{
    Node::free_wrappers(reinterpret_cast<xmlNode *>(document));
    xmlFreeDoc(document);
}

/* Implementation of "Parser" class */

Parser::Init::Init() { xmlInitParser(); }

Parser::Init::~Init() {}

Parser::Init Parser::init_;

Parser::Parser() : document(nullptr) { document = new Document(); }

Document *Parser::get_document() { return document; }

void Parser::parse_memory(const std::string &xmlStr) { document->parseXmlStr(xmlStr); }

void Parser::parse_file(const std::string &filePath) { document->parseXmlFile(filePath); }

Parser::operator bool() const { return document != nullptr; }

Parser::~Parser() { delete document; }

} // namespace xml
} // namespace pparam
