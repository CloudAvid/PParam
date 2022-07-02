/**
 * \file xml.hpp
 * This framework designed to manipulate XML.
 *
 * Our focus is on parameters of virtual machines to read, write, process,...
 * them from config repository.
 *
 * Copyright 2010,2022 Cloud Avid Co. (www.cloudavid.com)
 * \author Mehrdad Dashtbani (dashtbani@cloudavid.com)
 *
 * xml is part of PParam.
 *
 * PParam is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PParam is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PParam.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libxml/parser.h>
#include <string>
#include <vector>

namespace pparam
{
namespace xml
{

/**
 * \class Node.
 * this class is wrapper for xmlNode of libxml.
 */
class Node
{
public:
    typedef std::vector<Node *> NodeList;
    typedef std::vector<const Node *> const_NodeList;

    Node(xmlNode *_node);
    xmlNode *get_node() const;
    /**
     * @return node name.
     */
    std::string get_name() const;
    /**
     * @return children of node.
     */
    NodeList get_children(const std::string &name = std::string());
    const_NodeList get_children(const std::string &name = std::string()) const;
    /**
     * for every xmlNode* we create a Node and
     * assign to _private of xmlNode*,
     * this function realize type of xmlNode* and
     * create specific Node for it.
     */
    static void create_wrapper(xmlNode *_node);
    /**
     * this function release all Node (as recursively).
     */
    static void free_wrappers(xmlNode *_node);
    virtual ~Node();

private:
    /**
     * in fact this attribute play role of xml tag.
     */
    xmlNode *node;
};

/**
 * \class ElementNode.
 */
class Element : public Node
{
public:
    Element(xmlNode *node);
    /**
     * this function search on attribute of element and.
     * @return attribute value that attribute key is equals with input key.
     */
    std::string get_attribute(const std::string &key) const;
    ~Element() override;

private:
};

/**
 * \class ContentNode.
 */
class ContentNode : public Node
{
public:
    ContentNode(xmlNode *node);
    /**
     * @return content of node,
     * actually return this content inside tag (<>content</>).
     */
    std::string get_content() const;
    ~ContentNode() override;
};

/**
 * \class CommentNode.
 */
class CommentNode : public ContentNode
{
public:
    CommentNode(xmlNode *node);
    ~CommentNode() override;
};

/**
 * \class CDataNode.
 */
class CDataNode : public ContentNode
{
public:
    CDataNode(xmlNode *node);
    ~CDataNode() override;
};

/**
 * \class TextNode.
 */
class TextNode : public ContentNode
{
public:
    TextNode(xmlNode *node);
    ~TextNode() override;
};

/**
 * \class Document.
 * this class is a wrapper for xmlDoc of libxml.
 */
class Document
{
public:
    Document();
    ~Document();
    /**
     * @return root node of document.
     */
    Element *get_root_node();
    /**
     * parse xml string and initialize document.
     */
    void parseXmlStr(const std::string &xmlStr);
    /**
     * parse xml file and initialize document.
     */
    void parseXmlFile(const std::string &filePath);
    /**
     * Return xml as string.
     */
    std::string toString() const;
    /**
     * Execute query on document and return result.
     *
     * @note throws exception if any error occured.
     */
    std::string queryXml(const std::string &query, const std::string &partialTag = "");

    xmlDoc *get_document() const;

private:
    /**
     * this attribute play role of xml document.
     */
    xmlDoc *document;
};

/**
 * \class Parser.
 * this class is a wrapper for xmlParser of libxml.
 */
class Parser
{
    class Init
    {
    public:
        Init();
        ~Init();
    };

public:
    Parser();
    Document *get_document();
    /**
     * call xml string parser of document and initialize it.
     */
    void parse_memory(const std::string &xmlStr);
    /**
     * call xml file parser of documnet and initialize it.
     */
    void parse_file(const std::string &filePath);
    explicit operator bool() const;
    ~Parser();

private:
    /**
     * every parser need document for parse,
     * actually this document parsed whit parser.
     */
    Document *document;
    /**
     * an static instance for inner Init class.
     */
    static Init init_;
};

} // namespace xml
} // namespace pparam
