/**
 * \file xlist.hpp
 * Defines general list that manages concurrent access to nodes.
 *
 * Main focuse is to develope "rcu - kernel side specific list" like list.
 * So deletion of nodes of list would be done when all read accesses 
 * to that node been finished.
 * By start of deletion process, new access requests couldn't see deleted
 * node.
 * Read accesses in this list simulates by iterators on nodes.
 *
 * Copyright 2013 PDNSoft Co. (www.pdnsoft.com)
 * \author hamid jafarian (hamid.jafarian@pdnsoft.com)
 *
 * xlist is part of PParam.
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
 */
#ifndef _PDN_XLIST_HPP_
#define _PDN_XLIST_HPP_

#define GCC_VERSION (__GNUC__ * 10000 \
				+ __GNUC_MINOR__ * 100 \
				+ __GNUC_PATCHLEVEL__)

#include <list>
#if	GCC_VERSION > 40500 // check for GCC > 4.5
#include <atomic>
#else
#include <cstdatomic>
#endif

namespace pparam
{

template <typename T>
class XListNode;

template <typename T, typename _Alloc = std::allocator< XListNode<T> > >
class XList; 

template<typename T>
class XListIterator;
template<typename T>
class XListConstIterator;

/**
 * \class _XListNode_base
 * Common part of a node in %XList.
 *
 * _XListNode_base would record and care concurrent usage of data.
 */
class _XListNode_base
{
public:
	_XListNode_base *_M_next;
	_XListNode_base *_M_prev;
public:
	_XListNode_base() :  usageNO(0), is_erasing(false)
	{
		pthread_mutex_init(&n_lock, NULL);
		pthread_cond_init(&usage_cond, NULL);
		_M_next = _M_prev = this;
	}

	/** 
	 * New access to the node.
	 * Increment usgaeNO by "1".
	 * \see inc_usage
	 */
	void access()
	{
		inc_usage();
	}
	/** 
	 * End of access to the node.
	 * Decrement usageNO by "1".
	 * \see dec_usage
	 */
	void release()
	{
		dec_usage();
	}
	int get_usage()
	{
		return usageNO;
	}
	void lock()
	{
		pthread_mutex_lock(&n_lock);
	}
	void unlock()
	{
		pthread_mutex_unlock(&n_lock);
	}
	/**
	 * Start erasing process on this node.
	 * \return true: erase could be done, false: erase couldn't
	 * 	be done, because another erasing process is run.
	 *
	 * This function prevents concurrent erasing processes
	 * on one node.
	 */
	bool erasing()
	{
		if (is_erasing) {
			/* There is another erasing process on this node, 
			 * so erasing couldn't be continued.
			 */
			return false;
		} else {
			/* This is the first erasing process, so continue to
			 * erase.
			 */
			is_erasing = true;
			return true;
		} 
	}
	/**
	 * Change the next/prev pointers of this node.
	 * \param next pointer to next node, would be applied if not null.
	 * \param prev pointer to prev node, would be applied if not null.
	 */
	void chNextPrev(_XListNode_base *next, _XListNode_base *prev)
	{
		lock();
		if (next) _M_next = next;
		if (prev) _M_prev = prev;
		unlock();
	}
protected:
	/**
	 * Wait to finish all concurrent usages.
	 */
	void waitForUsage()
	{
		lock();
		while (usageNO > 0)
			pthread_cond_wait(&usage_cond, &n_lock);
		unlock();
	}
	/**
	 * Increment usageNO by "_inc".
	 * \param _inc number of increment, default is "1".
	 *
	 * We don't use any lock to increment usageNO.
	 * This beacause of atomic nature of "usageNO".
	 * Also wait operation on "usageNO" in "waitForUsage"
	 * would be done at deletion time, when no one could access him 
	 * to increment "usageNO", so "usageNO" never been increased
	 * at deletion time.
	 */
	void inc_usage(unsigned int _inc = 1)
	{
		usageNO += _inc;
	}
	/**
	 * Decrement usageNO by "_dec".
	 * \param _dec number of decrement, default is "1".
	 * \note Use this function when "this" has been locked.
	 * This because of "waitForUsage" ability to capture any decrement
	 * changes on "usageNO".
	 */
	void dec_usage(unsigned int _dec = 1)
	{
		usageNO -= _dec;
		if (usageNO <= 0) pthread_cond_signal(&usage_cond);
	}
	/**
	 * Number of concurrent usage of this node.
	 */
	std::atomic_uint usageNO;
	/**
	 * Lock to manage decrement changes of "usageNO".
	 *
	 * Also use to manage access next/prev nodes from "this".
	 * "n_lock" would be locked before (next/prev)pointer read, and 
	 * would be released after "access" to node pointed with 
	 * (next/prev) pointer.
	 * So between pointer read and node access, next/prev pointers 
	 * couldn't change.
	 */
	pthread_mutex_t n_lock; 
	/**
	 * Condition to monitor decrement changes of usageNO.
	 */
	pthread_cond_t usage_cond;
	/**
	 * Is erasing process in progress on this node?
	 */
	bool is_erasing;
};

/**
 * \class XListNode
 * Defines xlist node, containor of actual data.
 *
 * "T" is type of actual data in list.
 * "T" would support "operator=(const T &)" and copy constructor.
 */
template <typename T>
class XListNode : public _XListNode_base
{
	friend class XListIterator<T>;
	friend class XListConstIterator<T>;
	template <class TT, class AA> friend class XList;
public:
	typedef XListNode<T> 	_XListNode;

	XListNode(const T &_data) :
		_XListNode_base(),
		_M_data(_data)
	{ }

	T &get_data()
	{
		return _M_data;
	}

	~XListNode()
	{
		/* Before destructing target memory, wait to finish
		 * all concurrent usages to this node.
		 */
		waitForUsage();
	}

	/**
	 * Actual data.
	 */
	T _M_data;
};

/**
 * \class XListIterator
 * \brief general itarator for XList.
 *
 * Each iterate on nodes of list, means a new access to the data.
 * So when iterate reaches new node, he would access new node and
 * release previous.
 */
template <typename T>
class XListIterator 
{
	template <class TT, class AA> friend class XList;
	friend class XListConstIterator<T>;
public:
	typedef XListIterator<T>			_Self;
	typedef XListNode<T> 				_XNode;
	typedef _XListNode_base				_XNodeBase;

	typedef T    					value_type;
	typedef T*					pointer;
	typedef T&					reference;
	typedef std::bidirectional_iterator_tag		iterator_category;
	typedef ptrdiff_t				difference_type;

	XListIterator() 
	{
		_M_node = NULL;
		list_end = NULL;
		lockedNode = NULL;
	}
	explicit
	XListIterator(_XNodeBase *_list_end, _XNodeBase* __x)
	{
		lockedNode = NULL;
		_M_node = __x;
		list_end = _list_end;
		access();
	}
	/** 
	 * Copy constructor.
	 *
	 * Create a new copy of iteration, so we would 
	 * increment usage of target node.
	 */
	XListIterator(const _Self &xiter)
	{
		lockedNode = NULL;
		_M_node = xiter._M_node;
		list_end = xiter.list_end;
		access();
	}

	reference operator*() const
	{
		return static_cast<_XNode*>(_M_node)->_M_data;
	}
	pointer operator->() const
	{
		return &(static_cast<_XNode*>(_M_node)->_M_data);
	}
	_Self& operator++()
	{
		lock();
		release();
		_M_node = _M_node->_M_next;
		access();
		unlock();
		return *this;
	}
	_Self operator++(int)
	{
		lock();
		release();
		_Self __tmp = *this;
		_M_node = _M_node->_M_next;
		access();
		unlock();
	  	return __tmp;
	}
	_Self& operator--()
	{
		lock();
		release();
	 	_M_node = _M_node->_M_prev;
		access();
		unlock();
		return *this;
	}
	_Self operator--(int)
	{
		lock();
		release();
		_Self __tmp = *this;
		_M_node = _M_node->_M_prev;
		access();
		unlock();
		return __tmp;
	}
	/**
	 * Assign operation.
	 *
	 * Assign means "create a new copy of iteration", so we would 
	 * increment usage of target node.
	 */
	_Self& operator=(const _Self &rvalue)
	{
		/* Release previous node.
		 */
		lock();
		release();
		unlock();

		_M_node = rvalue._M_node;
		list_end = rvalue.list_end;
		access();
		return *this;
	}
	bool operator==(const _Self &rvalue) const
	{
		return _M_node == rvalue._M_node;
	}
	bool operator!=(const _Self &rvalue) const
	{
		return _M_node != rvalue._M_node;
	}
	/**
	 * Finalize the iteration.
	 *
	 * Release current node and set the node position to null.
	 */
	void fini()
	{
		lock();
		release();
		unlock();
		/* Going to the end of the list.
		 */
		this->_M_node = NULL;
		list_end = NULL;
	}
	/**
	 * Is this iterator finished? reached at end.
	 */
	bool is_finied()
	{
		return (this->_M_node == NULL);
	}
	/**
	 * Is this iterator finished? reached at end.
	 */
	bool is_end()
	{
		return (this->_M_node == list_end);
	}
	/**
	 * Do iterates on any node?
	 */
	bool on_element()
	{
		return ((this->_M_node != list_end) &&
				(this->_M_node != NULL));
	}
	bool on_node()
	{
		return ((this->_M_node != list_end) &&
				(this->_M_node != NULL));
	}
	/**
	 * Reset the iterator.
	 * After use of iterator for node deletion it would be resetted for
	 * future uses.
	 * Behavior of iterator in use after deletion without reset, is
	 * unpredictable (me seg-faut).
	 */
	void reset()
	{
		_M_node = NULL;
		list_end = NULL;
		lockedNode = NULL;
	}
	~XListIterator()
	{
		fini();
	}

protected:
	/** 
	 * Lock the target node.
	 */
	void lock()
	{
		if (_M_node) {
			lockedNode = _M_node;
			lockedNode->lock();
		}
	}
	/**
	 * Unlock the locked node.
	 */
	void unlock()
	{
		if (lockedNode) {
			lockedNode->unlock();
			lockedNode = NULL;
		}
	}
	/**
	 * Release the node that iterates on.
	 */
	void release()
	{
		if (_M_node) _M_node->release();
	}
	/**
	 * Access to the node that iterates on.
	 */
	void access()
	{
		if (_M_node) _M_node->access();
	}
	/**
	 * Pointer to the end of list that "this" iterates on.
	 */
	_XNodeBase *list_end;
	/**
	 * Pointer to locked node.
	 */
	_XNodeBase *lockedNode;
	/**
	 * pointer to the list node that "this" iterates on.
	 */
	_XNodeBase* _M_node;
};

/**
 * \class XListConstIterator
 * \brief const itarator for XList.
 *
 * Each iterate on nodes of list, means a new access to the data.
 * So when iterate reaches new node, he would access new node and
 * release previous.
 */
template <typename T>
class XListConstIterator 
{
	template <class TT, class AA> friend class XList;
public:
	typedef XListConstIterator<T>			_Self;
	typedef XListIterator<T>			_XListIterator;
	typedef XListNode<T> 				_XNode;
	typedef _XListNode_base				_XNodeBase;

	typedef T    					value_type;
	typedef const T*				pointer;
	typedef const T&				reference;
	typedef std::bidirectional_iterator_tag		iterator_category;
	typedef ptrdiff_t				difference_type;

	XListConstIterator() 
	{
		_M_node = NULL;
		list_end = NULL;
		lockedNode = NULL;
	}
	explicit
	XListConstIterator(const _XNodeBase *_list_end, 
					const _XNodeBase* __x)
	{
		lockedNode = NULL;
		_M_node = __x;
		list_end = _list_end;
		access();
	}
	/** 
	 * Copy constructor.
	 */
	XListConstIterator(const _Self &xciter)
	{
		lockedNode = NULL;
		_M_node = xciter._M_node;
		list_end = xciter.list_end;
		access();
	}
	/**
	 * Copy constructor for general iterator.
	 */
	XListConstIterator(const _XListIterator &xiter)
	{
		lockedNode = NULL;
		_M_node = xiter._M_node;
		list_end = xiter.list_end;
		access();
	}

	reference operator*() const
	{
		return static_cast<const _XNode*>(_M_node)->_M_data;
	}
	pointer operator->() const
	{
		return &(static_cast<const _XNode*>(_M_node)->_M_data);
	}
	_Self& operator++()
	{
		lock();
		release();
		_M_node = _M_node->_M_next;
		access();
		unlock();
		return *this;
	}
	_Self operator++(int)
	{
		lock();
		release();
		_Self __tmp = *this;
		_M_node = _M_node->_M_next;
		access();
		unlock();
	  	return __tmp;
	}
	_Self& operator--()
	{
		lock();
		release();
	 	_M_node = _M_node->_M_prev;
		access();
		unlock();
		return *this;
	}
	_Self operator--(int)
	{
		lock();
		release();
		_Self __tmp = *this;
		_M_node = _M_node->_M_prev;
		access();
		unlock();
		return __tmp;
	}
	/**
	 * Assign operation.
	 *
	 * Assign means "create a new copy of iteration", so we would 
	 * increment usage of target node.
	 */
	_Self& operator=(const _Self &rvalue)
	{
		/* Release previous node.
		 */
		lock();
		release();
		unlock();

		_M_node = rvalue._M_node;
		list_end = rvalue.list_end;
		access();
		return *this;
	}
	bool operator==(const _Self &rvalue)
	{
		return _M_node == rvalue._M_node;
	}
	bool operator==(const _XListIterator &rvalue)
	{
		return *this == _Self(rvalue);
	}
	bool operator!=(const _Self &rvalue) const
	{
		return _M_node != rvalue._M_node;
	}
	bool operator!=(const _XListIterator &rvalue) const
	{
		return *this != _Self(rvalue);
	}
	/**
	 * Finalize the iteration.
	 *
	 * Release current node and set node position to null.
	 */
	void fini()
	{
		lock();
		release();
		unlock();
		/* Going to the end of the list.
		 */
		this->_M_node = NULL;
		this->list_end = NULL;
	}
	/**
	 * Is this iterator finished? 
	 */
	bool is_finied()
	{
		return (this->_M_node == NULL);
	}
	/**
	 * Is this iterator finished? reached at end.
	 */
	bool is_end()
	{
		return (this->_M_node == list_end);
	}
	/**
	 * Do iterates on any node?
	 */
	bool on_element()
	{
		return ((this->_M_node != list_end) &&
				(this->_M_node != NULL));
	}
	bool on_node()
	{
		return ((this->_M_node != list_end) &&
				(this->_M_node != NULL));
	}
	/**
	 * Reset the iterator.
	 * After use of iterator for node deletion it would be resetted for
	 * future uses.
	 * Behavior of iterator in use after deletion without reset, is
	 * unpredictable (me seg-faut).
	 */
	void reset()
	{
		_M_node = NULL;
		list_end = NULL;
		lockedNode = NULL;
	}
	~XListConstIterator()
	{
		fini();
	}

protected:
	/** 
	 * Lock the target node.
	 */
	void lock()
	{
		if (_M_node) {
			lockedNode = (_XNodeBase *)_M_node;
			lockedNode->lock();
		}
	}
	/**
	 * Unlock the locked node.
	 */
	void unlock()
	{
		if (lockedNode) {
			lockedNode->unlock();
			lockedNode = NULL;
		}
	}
	/**
	 * Release the node that iterates on.
	 */
	void release()
	{
		if (_M_node) ((_XNodeBase *)_M_node)->release();
	}
	/**
	 * Access to the node that iterates on.
	 */
	void access()
	{
		if (_M_node) ((_XNodeBase *)_M_node)->access();
	}
	/**
	 * Pointer to end node of list that "this" iterates on.
	 */
	const _XNodeBase *list_end;
	/**
	 * Pointer to locked node.
	 */
	_XNodeBase *lockedNode;
	/**
	 * pointer to the list node that "this" iterates" on.
	 */
	const _XNodeBase *_M_node;
};


template <typename T, typename _Alloc>
class _XList_base
{
protected:
	typedef typename _Alloc::template rebind<XListNode<T>>::other
		_Node_alloc_type;
	typedef typename _Alloc::template rebind<T>::other _T_alloc_type;

	struct _XList_impl : public _Node_alloc_type
	{
		_XListNode_base _M_node;

		_XList_impl()
		: _Node_alloc_type(), _M_node()
		{ }

		_XList_impl(const _Node_alloc_type& __a)
		: _Node_alloc_type(__a), _M_node()
		{ }

	#ifdef __GXX_EXPERIMENTAL_CXX0X__
		_XList_impl(_Node_alloc_type&& __a)
		: _Node_alloc_type(std::move(__a)), _M_node()
		{ }
	#endif

		void lock()
		{
			_M_node.lock();
		}
		void unlock()
		{
			_M_node.unlock();
		}
	};

	_XList_impl _M_impl;

	XListNode<T>* _M_get_node()
	{
		return _M_impl._Node_alloc_type::allocate(1);
	}
	void _M_put_node(XListNode<T> *__p)
	{
		_M_impl._Node_alloc_type::deallocate(__p, 1);
	}

public:
	typedef _Alloc allocator_type;

	_Node_alloc_type& _M_get_Node_allocator()
	{
		return *static_cast<_Node_alloc_type *>(&_M_impl);
	}
	const _Node_alloc_type& _M_get_Node_allocator() const
	{
		return *static_cast<const _Node_alloc_type *>(&_M_impl);
	}
	_T_alloc_type _M_get_T_allocator() const
	{
		return _T_alloc_type(_M_get_Node_allocator());
	}

	_XList_base() : _M_impl()
	{ }
	_XList_base(const _Node_alloc_type& __a) : _M_impl(__a)
	{ }
};

/**
 * \class XList
 * "rcu" like list to manage reads(concurrent accesses) without whole 
 * list lock.
 *
 * In XList, reads(concurrent accesses) to any node of list means 
 * "iterators" have been located on that node.
 *
 * XList supports cocurrently multiple reads with one write at a time.
 *
 * Erase of nodes from list could be done in two steps:
 * \code
 * 	XList list;
 * 	XList::iterator iter;
 * 	// Critical Section begin
 *	bool ret = list.xerase_prepare(iter);
 *	// Critical Section end
 *	if (ret) list.xerase(iter);
 * \endcode
 * "xerase_prepare" would remove node from list, so after preparation,
 * no one could see(access) erased node.
 * "xerase" would free node and would be bloked untill all concurrent
 * accesses finished.
 */
template <typename T, typename _Alloc >
class XList : public _XList_base<T, _Alloc> 
{
public:
	typedef XList<T, _Alloc>			_XList;
	typedef XListIterator<T>			iterator;
	typedef XListConstIterator<T>			const_iterator;
	typedef std::reverse_iterator<iterator>		reverse_iterator;
	typedef std::reverse_iterator<const_iterator>	const_reverse_iterator;
	typedef XListNode<T>				_XNode;
	typedef _XListNode_base				_XNodeBase;
	typedef _XList_base<T, _Alloc>			_XBase;
	typedef T					value_type;
	typedef size_t					size_type;

protected:
	using _XBase::_M_get_Node_allocator;
	using _XBase::_M_impl;
	using _XBase::_M_put_node;

#ifndef __GXX_EXPERIMENTAL_CXX0X__
	_XNode* _M_create_node(const value_type& __x)
	{
		_XNode* __p = this->_M_get_node();
		__try {
			_M_get_T_allocator().construct
				(std::__addressof(__p->_M_data), __x);
		} __catch(...) {
    			_M_put_node(__p);
    			__throw_exception_again;
		}
		return __p;
	}
#else
      /**
       *  \param  __args  An instance of user data.
       *
       *  Allocates space for a new node and constructs a copy of
       *  @a __args in it.
       */
	template<typename... _Args>
	_XNode* _M_create_node(_Args&&... __args)
	{
		_XNode* __p = this->_M_get_node();
		__try {
			_M_get_Node_allocator().construct(__p,
				std::forward<_Args>(__args)...);
		} __catch(...) {
			_M_put_node(__p);
			__throw_exception_again;
		}
		return __p;
	}
#endif

public:
	XList() : _XList_base<T, _Alloc>()
	{ }
	XList(const _XList &xlist) : _XList_base<T, _Alloc>()
	{	
		for (const_iterator iter = xlist.begin();
					iter != xlist.end(); ++iter) {
			push_back(*iter);
		}
	}
	iterator begin()
	{
		this->_M_impl.lock();;
		iterator iter(&this->_M_impl._M_node, 
					this->_M_impl._M_node._M_next);
		this->_M_impl.unlock();;
		return iter;
	}
	const_iterator begin() const
	{
		/* Excuse Me ..!! */
		((_XList *)this)->_M_impl.lock();
		const_iterator iter(&this->_M_impl._M_node, 
					this->_M_impl._M_node._M_next);
		((_XList *)this)->_M_impl.unlock();
		return iter;
	}
	iterator end()
	{
		return iterator(&this->_M_impl._M_node, 
					&this->_M_impl._M_node);
	}
	const_iterator end() const
	{
		return const_iterator(&this->_M_impl._M_node, 
						&this->_M_impl._M_node);
	}
	reverse_iterator rbegin() 
	{
		return reverse_iterator(end());
	}
	const_reverse_iterator rbegin() const
	{
		return const_reverse_iterator(end());
	}
	reverse_iterator rend()
	{
		return reverse_iterator(begin());
	}
	const_reverse_iterator rend() const
	{
		return const_reverse_iterator(begin());
	}
	const_iterator cbegin() const
	{
		return const_iterator(begin());
	}
	const_iterator cend() const
	{
		return const_iterator(end());
	}
	const_reverse_iterator crbegin() const
	{
		return const_reverse_iterator(cend());
	}
	const_reverse_iterator crend() const
	{
		return const_reverse_iterator(cbegin());
	}
	/**
	 * Add data to end of the list.
	 */
	void push_back(const value_type &val)
	{
		_XNode *_node = _M_create_node(val);
		/* next points to end */
		_node->_M_next = &this->_M_impl._M_node;
		/* prev points to the prev node of end */
		_node->_M_prev = this->_M_impl._M_node._M_prev;
		_node->access();
		_M_impl._M_node._M_prev->chNextPrev((_XNodeBase *)_node, NULL);
		_node->access();
		_M_impl._M_node.chNextPrev(NULL, (_XNodeBase *)_node);
	}
	/**
	 * Delete last node of list.
	 */
	void pop_back()
	{
		erase(-- end());
	}
	/**
	 * Erases node at given position.
	 *
	 * By nature of XList, we would care concurrent accesses to target
	 * node. 
	 * Erase would be done in two step (function call), 
	 * 	1: xerase_prepare(pos)
	 * 	2: xerase(pos)
	 * 
	 * xerase_prepare would remove pointers to target node:
	 * \code
	 * 	Before xerase_prepare of "B": A <--> B <--> C <--> D
	 * 	After  xerase_prepare of "B": A <-----> C <--> D
	 * 			                <--B-->
	 * \endcode
	 * So after prepare, list iterations couldn't see target node ("B").
	 * But any iteration on this node ("B") can go back/forward.
	 * Beacause of one write/delete operation at a time on XList, 
	 * you could position this function in your critical section
	 * and protect him by lock.
	 * After preparation, "xerase" call could be done out of critical 
	 * section. So actual deletion of target node wouldn't block your
	 * critical section.
	 * "xerase" would wait to finish all concurent accesses to target 
	 * node, when all accesses finished, node would be erased.
	 */
	bool xerase_prepare(iterator pos)
	{
		/* Check to see can erase this node? 
		 */
		if (! pos._M_node->erasing()) return false;
		/* Change prev pointer of next node.
		 * Because of pos._M_prev points to the prev node, we should
		 * access him. there is one more pointer to this node.
		 * Beacause of one write at a time, access without lock on
		 * node.
		 */
		pos._M_node->_M_prev->access();
		pos._M_node->_M_next->chNextPrev(NULL, pos._M_node->_M_prev);
		/* Release without lock, because we are before of "xerase",
		 * So there is no waiting for uasge.
		 */
		pos._M_node->release();
		/* Change next pointer of prev node.
		 * Because of pos._M_next points to the next node, we should
		 * access him. there is one more pointer to this node.
		 * Beacause of one write at a time, access without lock on
		 * node
		 */
		pos._M_node->_M_next->access();
		pos._M_node->_M_prev->chNextPrev(pos._M_node->_M_next, NULL);
		/* Release without lock, because we are before of "xerase",
		 * So there is no waiting for uasge.
		 */
		pos._M_node->release();
		return true;
	}
	/** 
	 * Destroy node.
	 * Destructor of the node would wait to finish all concurrent 
	 * accesses to the node.
	 * By the block nature of this function, it 's appropriate to call and 
	 * use XList in multithreaded environment.
	 */
	void xerase(iterator pos)
	{
		_XNode *__n = static_cast<_XNode*>(pos._M_node);
		_XNodeBase *__M_next = __n->_M_next;
		_XNodeBase *__M_prev = __n->_M_prev;
		/* Decrement usage by "one" because There is another iterator 
		 * on this node that "pos" is copy of that.
		 */
		pos._M_node->release();
		/* Finalize iteration.
		 */
		pos.fini();
		_M_get_Node_allocator().destroy(__n);
		/* Node destroyed, so release next/prev nodes.
		 */
		__M_next->lock();
		__M_next->release();
		__M_next->unlock();
		__M_prev->lock();
		__M_prev->release();
		__M_prev->unlock();
	}
	/**
	 * Call of "xerase-prepare" & "xerase" in one function call.
	 */
	void erase(iterator pos)
	{
		if (xerase_prepare(pos)) {
			/* Decrement usage by "one" because There is 
			 * another iterator on this node that "pos" is 
			 * copy of that.
			 */
			pos._M_node->release();
			xerase(pos);
		}
	}
	void clear()
	{
		iterator iter;
		while ((iter = begin()) != this->end()) {
			erase(iter);
		}
	}
	/**  
	 * Returns the number of elements in the %XList.  
	 */
	size_type size() const
	{
		return std::distance(begin(), end()); 
	}
	~XList()
	{
		clear();
	}
};

} // namespace pparam

#endif // _PDN_XLIST_HPP_

