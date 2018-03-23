#pragma once
#include <functional>
#include <memory>

template<class Alloc, class Type>
using RebindAllocator = typename std::allocator_traits<Alloc>::template rebind_alloc<Type>; // Move this into a traits file for containers?

template<class Tree>
class TreeIterator
{
	using iterator_category = std::bidirectional_iterator_tag;
	using NodePtr		    = typename Tree::NodePtr;
	using difference_type   = typename Tree::difference_type;
	using value_type        = typename Tree::value_type;
	using pointer		    = typename Tree::pointer;
	using reference		    = typename value_type&;

public:
	TreeIterator() = default;
	TreeIterator(NodePtr node, const Tree *tree) : node(node), tree(tree)
	{
	}

	reference operator*() 
	{
		return node->data;
	}

	pointer operator->() 
	{
		return std::pointer_traits<pointer>::pointer_to(**this);
	}

	bool operator==(const TreeIterator& i) const
	{   // Compare ptr's not data itself
		return node == i.node; 
	}

	bool operator!=(const TreeIterator& i) const
	{
		return !(this->node == i.node);
	}

	TreeIterator& operator++() // Preincrement
	{
		if (this->node == tree->Head) // Don't increment on null/Head node
			;
		else if (node->subtree[1]) // Right node is non empty, find smallest member
		{
			node = Tree::minNode(node->subtree[1]);
		}
		else // Climb up tree looking for first Node with a non-empty right subtree
		{
			NodePtr n;
			// While node has a parent and the iterator is equal
			// to its parents right node, traverse up tree
			while ((n = node->parent) && node == n->subtree[1])
				node = n;

			bool found = false;
			if (n)
			{
				node = n;
				found = true;
			}

			// Inelegant way to set Iterator to end
			if (!found)  // How to replace?
				*this = TreeIterator(tree->Head, tree);
		}
		return *this;
	}

	TreeIterator operator++(int) // Postincrement
	{
		TreeIterator i = *this;
		++*this;
		return i;
	}

	TreeIterator& operator--()
	{
		if (this->node == tree->Head)
			;
		else if (node->subtree[0]) // Left node is non empty, find largest memeber
		{
			node = Tree::maxNode(node->subtree[0]);
		}
		else
		{
			NodePtr n;
			while ((n = node->parent) && node == n->subtree[0])
				node = n;

			if (n)
			{
				node = n;
			}
			else
				*this = TreeIterator(tree->Head, tree);

		}
		return *this;
	}

	TreeIterator operator--(int)
	{
		TreeIterator i = *this;
		--*this;
		return i;
	}
protected:
	NodePtr node = nullptr; 
	const Tree * tree;
	friend Tree;
};

template<class Tree>
class ConstTreeIterator : public TreeIterator<Tree> // TODO: At the moment this is basically identical to TreeIterator, make other values const once support is added for pairs, etc
{
	using MyBase     = TreeIterator<Tree>;
	using value_type = typename Tree::value_type; 
	using NodePtr	 = typename Tree::NodePtr;			// Inaccessible Types ??? Compiler warning
	using pointer    = typename Tree::const_pointer;
	using reference  = typename Tree::const_reference;
	friend class MyBase;

public:

	ConstTreeIterator() : MyBase() {}
	ConstTreeIterator(const MyBase& b) : MyBase(b) {}
	ConstTreeIterator(NodePtr node, const Tree *tree) : MyBase(node, tree) {}

	reference operator*() const
	{
		return this->node->data;
	}

	pointer operator->() const
	{
		return std::pointer_traits<pointer>::pointer_to(**this);
	}
};

template<class Type>
struct Node
{
	Node* parent;
	Node* subtree[2]; 

	char color; 
	Type data;
};


// Wrapper for types needed by tree and iterators
template<class Traits>
struct TreeTypes
{
	using Alloc = typename Traits::allocator_type;
	using key_type = typename Traits::key_type;
	using key_compare = typename Traits::key_compare;

	using node_type = typename Traits::node_type;
	using Node = Node<node_type>;

	using NodeAl = RebindAllocator<Alloc, Node>;
	using NodeAlTraits = std::allocator_traits<NodeAl>;
	using AllocTraits  = std::allocator_traits<Alloc>;

	using NodePtr		  = typename NodeAlTraits::pointer;
	using difference_type = typename NodeAlTraits::difference_type;
	using value_type	  = typename AllocTraits::value_type;       
	using pointer		  = typename AllocTraits::pointer;
	using const_pointer   = typename AllocTraits::const_pointer;
	using reference		  = value_type&;
	using const_reference = const value_type&;
};

template<class Traits>
class RedBlackTree
{
	using MyBase = RedBlackTree<Traits>;
	using BaseTypes = TreeTypes<Traits>;

	enum Color { RED, BLACK };
	enum Direction { LEFT, RIGHT };

	using Node			  = typename BaseTypes::Node;
	using NodePtr		  = typename BaseTypes::NodePtr;
	using difference_type = typename BaseTypes::difference_type;
	using value_type	  = typename BaseTypes::value_type;
	using pointer		  = typename BaseTypes::pointer;
	using const_pointer   = typename BaseTypes::const_pointer;
	using reference		  = typename BaseTypes::reference;
	using const_reference = typename BaseTypes::const_reference;

	using key_type		= typename BaseTypes::key_type;
	using key_compare   = typename Traits::key_compare;
	using value_compare = typename Traits::value_compare;
	using get_key		= typename Traits::get_key;

	using NodeAl	    = typename BaseTypes::NodeAl;
	using NodeAlTraits  = typename BaseTypes::NodeAlTraits;

	NodeAl nodeAl; // Should this only be init on node construct/destruct?

	friend class ConstTreeIterator<MyBase>;
	friend class TreeIterator<MyBase>;

	using Iterator = std::conditional_t<std::is_same<key_type, value_type>::value, ConstTreeIterator<MyBase>, TreeIterator<MyBase>>;

	using reverse_iterator = std::reverse_iterator<Iterator>;
	using Const_Iterator   = ConstTreeIterator<MyBase>;

	using ItBool = std::pair<Iterator, bool>;

private:
	NodePtr Head;
	NodePtr root = nullptr;
	long treeSize = 0;

	// If we're working with key/value pair extract the value of the key
	// from the pairs and compare the two
	value_compare compare;

	// Get a key from a node data type
	get_key getKey;

	// Compare keys directly
	key_compare keyCompare;

public:

	RedBlackTree()
	{
		createHead();
	}

	Iterator begin() noexcept { return Iterator(lMost(), this); } // Replace lMost() with a cached left value instead of looking it up!!
	Iterator end() noexcept { return Iterator(Head, this); }
	Const_Iterator begin() const noexcept { return Const_Iterator(lMost(), this); } // Replace lMost()
	Const_Iterator end() const noexcept { return Const_Iterator(Head, this); }

	Const_Iterator cbegin() const noexcept { return begin(); }
	Const_Iterator cend() const noexcept { return end(); }

	reverse_iterator rbegin() noexcept { return std::make_reverse_iterator(Iterator(rMost(), this)); } // TODO: Cache rMost() in Head->subtree[RIGHT] (actually just passing head with subtree attached to rMost)
	reverse_iterator rend() noexcept { return std::make_reverse_iterator(begin()); }

private:

	void createHead()
	{
		NodePtr n = nodeAl.allocate(1);
		NodeAlTraits::construct(nodeAl, std::addressof(n->subtree[0]), nullptr);
		NodeAlTraits::construct(nodeAl, std::addressof(n->subtree[1]), nullptr); // TODO: Try - Catch
		NodeAlTraits::construct(nodeAl, std::addressof(n->parent), nullptr);

		n->color = BLACK;
		Head = n; // Heads right needs to be rmost, left needs to be lmost
	}

public:

	NodePtr allocateNode()
	{
		NodePtr n = nodeAl.allocate(1); // Should allocater be instantiated here?
		NodeAlTraits::construct(nodeAl, std::addressof(n->subtree[0]), nullptr); // Are these needed?
		NodeAlTraits::construct(nodeAl, std::addressof(n->subtree[1]), nullptr); // TODO: throw on allocation failue?
		NodeAlTraits::construct(nodeAl, std::addressof(n->parent), nullptr);

		return n;
	}

	template<class... Val>
	NodePtr createNode(Val&&... val)
	{
		NodePtr n = allocateNode();

		n->color = RED;
		NodeAlTraits::construct(nodeAl, std::addressof(n->data), std::forward<Val>(val)...);

		// TODO: throw / catch on failed construction

		return n;
	}

	template<class... Val>
	Iterator emplace(Val&&... val)
	{
		NodePtr n = createNode(std::forward<Val>(val)...);

		return tryAddNode(n);
	}

private:

	Iterator tryAddNode(NodePtr n) // TODO: Add exception handling?
	{							 // TODO: Add in bool template param for identical keys
		if (root == nullptr)
		{
			Head->parent = root = n;
		}
		else
		{
			int dir;
			NodePtr c = root;

			for (;;)
			{
				dir = compare(c->data, n->data);

				if (c->data == n->data) // Don't overwrite here TODO: Add in bool template param for taking params of same value
				{						// TODO: Get rid of operator == neccesity?
					freeNode(n);
					return Iterator(Head, this);
				}
				// Break when leaf node is found in comparative direction
				else if (c->subtree[dir] == nullptr)
					break;

				c = c->subtree[dir]; // Navigate down tree
			}

			c->subtree[dir] = n;
			c->subtree[dir]->parent = c;

			bottomUpInsertion(n);
			// TODO: Add in top down insertion and test benifits
		}

		root->color = BLACK;
		++treeSize;

		//testTree(root);
		return Iterator { n, this };
	}

	void bottomUpInsertion(NodePtr child)
	{
		if (!child->parent)
			throw std::runtime_error("Insertion node has no parent"); // Delete this once done testing


																	  // We need to fix tree for children with red parents
		for (NodePtr c = child; c->parent->color == RED;)
		{
			const int dir = (c->parent == c->parent->parent->subtree[RIGHT]); // Change to int once done debugging

																			  // If sister node is red attempt a re-color
			NodePtr uncle = c->parent->parent->subtree[!dir];

			if (uncle && uncle->color == RED)
			{
				c->parent->color = uncle->color = BLACK; // Color parent and it's uncle node black
				c->parent->parent->color = RED;  // Color grandparent red
				c = c->parent->parent;			 // Set current node to grandparent

				if (!c->parent) // Delete this once Head is added in 
					break;		// (BLACK parent of root & pointed to by all null nodes)
			}

			// Parent has red and black children
			else
			{
				// If this node is (dir) child
				// rotate on node to the !(dir)
				if (c == c->parent->subtree[!dir])
				{
					c = c->parent;
					rotateDir(c, dir);
				}

				c->parent->color = BLACK;
				c->parent->parent->color = RED;
				rotateDir(c->parent->parent, !dir);
			}
		}
	}

	void freeNode(NodePtr n)
	{
		NodeAlTraits::destroy(nodeAl, std::addressof(n->subtree[LEFT]));
		NodeAlTraits::destroy(nodeAl, std::addressof(n->subtree[RIGHT]));
		NodeAlTraits::destroy(nodeAl, std::addressof(n->parent));
		NodeAlTraits::deallocate(nodeAl, n, 1); // TODO: Deallocation should be done when obj destructor is called like STL containers?
	}

public:

	Iterator erase(const key_type& t) // Return iterators to next element
	{
		Const_Iterator td = find(t);

		return td != end() ? erase(td) : end();
	}

	Iterator erase(const Const_Iterator& it)
	{
		Const_Iterator successor = it;
		++successor;

		NodePtr eraseNode = deleteElement(it);

		NodeAlTraits::destroy(nodeAl, std::addressof(eraseNode));
		freeNode(eraseNode);

		//testTree(this->root);
		return Iterator(successor.node, this);
	}

private:

	NodePtr deleteElement(Const_Iterator it) // change to const iterator
	{
		// Save successor iterator for return
		NodePtr eraseNode = it.node;
		++it;

		NodePtr pnode = eraseNode;
		NodePtr fixParent;
		NodePtr fixNode;

		if (!pnode->subtree[LEFT])
			fixNode = pnode->subtree[RIGHT];

		else if (!pnode->subtree[RIGHT])
			fixNode = pnode->subtree[LEFT];

		else
		{
			pnode = it.node;
			fixNode = pnode->subtree[RIGHT];
		}

		// Only one subtree
		if (pnode == eraseNode)
		{
			fixParent = eraseNode->parent;

			if (fixNode)
				fixNode->parent = fixParent;

			if (this->root == eraseNode)
			{
				this->root = fixNode;
			}
			else
			{
				const int tdir = fixParent->subtree[RIGHT] == eraseNode;
				fixParent->subtree[tdir] = fixNode;
			}

			// Add left most and right most caching here
		}

		else
		{
			eraseNode->subtree[LEFT]->parent = pnode;
			pnode->subtree[LEFT] = eraseNode->subtree[LEFT];

			if (pnode == eraseNode->subtree[RIGHT]) // Successor is next to erased node
				fixParent = pnode;

			else
			{
				fixParent = pnode->parent;
				if (fixNode)
					fixNode->parent = fixParent;

				fixParent->subtree[LEFT] = fixNode;
				pnode->subtree[RIGHT] = eraseNode->subtree[RIGHT];
				eraseNode->subtree[RIGHT]->parent = pnode;
			}

			if (this->root == eraseNode)
				this->root = pnode;

			else
			{
				int tdir = eraseNode->parent->subtree[RIGHT] == eraseNode;
				eraseNode->parent->subtree[tdir] = pnode;
			}

			pnode->parent = eraseNode->parent;
			std::swap(pnode->color, eraseNode->color);
		}

		if (eraseNode->color == BLACK) // Have to recolor tree when erasing non-red parent || child
		{
			for (; fixNode != this->root && (!fixNode || fixNode->color == BLACK); fixParent = fixNode->parent)
			{
				const int tdir = fixNode == fixParent->subtree[RIGHT];

				// Fixup tdir subtree
				pnode = fixParent->subtree[!tdir];

				if (pnode->color == RED) // rotate red up from !tdir subtree
				{
					pnode->color = BLACK;
					fixParent->color = RED;
					rotateDir(fixParent, tdir);
					pnode = fixParent->subtree[!tdir];
				}

				if (!pnode)
					fixNode = fixParent;

				else if ((!pnode->subtree[tdir] || pnode->subtree[tdir]->color == BLACK)
					&& (!pnode->subtree[!tdir] || pnode->subtree[!tdir]->color == BLACK)) // Redden right subtree that has two black(or null) children
				{
					pnode->color = RED;
					fixNode = fixParent;
				}
				else
				{ // Rearrange !tdir subtree
					if ((!pnode->subtree[!tdir] || pnode->subtree[!tdir]->color == BLACK))
					{   // Rotate red up from tdir subtree with a !tdir rotation
						pnode->subtree[tdir]->color = BLACK;
						pnode->color = RED;
						rotateDir(pnode, !tdir);
						pnode = fixParent->subtree[!tdir];
					}

					pnode->color = fixParent->color;
					fixParent->color = BLACK;
					pnode->subtree[!tdir]->color = BLACK;
					rotateDir(fixParent, tdir);
					break;
				}
			}

			if(fixNode)
				fixNode->color = BLACK;
		}

		if (treeSize > 0)
			--treeSize;

		return eraseNode;
	}

public:

	Iterator find(const key_type& t) 
	{
		NodePtr p = lowerBound(t);

		return Iterator(p, this);
	}

	Const_Iterator find(const key_type &t) const
	{
		return Const_Iterator(find(t));
	}

private:

	// Find the left most node not less than t
	NodePtr lowerBound(const key_type& t) const
	{
		NodePtr n = this->root;
		NodePtr best = this->Head;

		while (n)
		{
			const int dir = keyCompare(getKey(n->data), t);

			if (!dir)
				best = n;

			n = n->subtree[dir];
		}

		return best;
	}

	static NodePtr minNode(NodePtr p)
	{
		while (p->subtree[LEFT])
			p = p->subtree[LEFT];

		return p;
	}

	static NodePtr maxNode(NodePtr p)
	{
		while (p->subtree[RIGHT])
			p = p->subtree[RIGHT];
		return p;
	}

	NodePtr lMost() const  // TODO: Make these return cached values
	{
		NodePtr p = this->root;

		while (p->subtree[LEFT])
			p = p->subtree[LEFT];

		return p;
	}

	NodePtr rMost() const
	{
		NodePtr p = this->root;
		while (p->subtree[RIGHT])
			p = p->subtree[RIGHT];
		return p;
	}

	void rotateDir(NodePtr root, const int dir)
	{
		NodePtr newRoot = root->subtree[!dir];
		root->subtree[!dir] = newRoot->subtree[dir];

		if (newRoot->subtree[dir])
			newRoot->subtree[dir]->parent = root;

		newRoot->parent = root->parent;

		if (root == this->root)
			this->root = newRoot;

		else
			root->parent->subtree[(root == root->parent->subtree[dir]) ? dir : !dir] = newRoot; // This branch can be avoided if we pass a third param, since it's calculated in bottomupinsertion
																								// Worth it or not?
		newRoot->subtree[dir] = root;
		root->parent = newRoot;
	}

	bool isRed(NodePtr n)
	{
		return (n && n->color == RED);
	}

	int testTree(NodePtr root)
	{
		if (!root || root == Head)
			return 1;

		NodePtr left = root->subtree[LEFT];
		NodePtr right = root->subtree[RIGHT];

		// Red node has a red child
		if (isRed(root))
			if (isRed(left) || isRed(right))
			{
				throw std::runtime_error("Red Violation!");
				return 0;
			}

		int lHeight = testTree(left);
		int rHeight = testTree(right);

		if ((left && left->data >= root->data) || (right && right->data <= root->data)) // This does not use Compare!!!
		{
			throw std::runtime_error("Invalid Binary Tree!");
			return 0;
		}

		if (lHeight && rHeight && lHeight != rHeight)
		{
			throw std::runtime_error("Black Height mis-match!");
			return 0;
		}

		// Only cound black links
		if (lHeight && rHeight)
			return isRed(root) ? lHeight : lHeight + 1;

		return 0;
	}
};