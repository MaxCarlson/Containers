#pragma once
#include <functional>
#include <memory>

template<class Alloc, class Type>
using RebindAllocator = typename std::allocator_traits<Alloc>::template rebind_alloc<Type>; // Move this into a traits file for containers?

template<class Tree>
class TreeIterator
{
	using NodePtr = typename Tree::Node*;
	using pointer = const NodePtr;
	using reference = typename Tree::reference;

public:
	TreeIterator() = default;
	TreeIterator(NodePtr node, Tree *tree) : node(node), tree(tree)
	{
	}

	pointer operator->() 
	{
		return std::pointer_traits<pointer>::pointer_to(**this);
	}

	reference operator*() 
	{
		return node->data;
	}

	bool operator==(const TreeIterator& i) const
	{
		return node == i.node; // Compare ptr's not data itself
	}

	bool operator!=(const TreeIterator& i) const
	{
		return !(this->node == i.node);
	}

	TreeIterator& operator++() // Preincrement
	{
		if (this->node == tree->Head) // Don't increment on null node, usually end()
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
			if (!found)
				*this = TreeIterator(tree->Head, tree); // How to replace?
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
		if (!data)
			;
		else if (node->subtree[0]) // Left node is non empty, find largest memeber
		{
			node = Tree::maxNode(node->subtree[0]);
			data = &node->data;
		}
		else
		{
			NodePtr n;
			while ((n = node->parent) && node == n->subtree[0])
				node = n;

			if (n)
			{
				node = n;
				data = &node->data;
			}
			else
				*this = tree->Head;

		}
		return *this;
	}

	TreeIterator operator--(int)
	{
		Iterator i = *this;
		--*this;
		return i;
	}


	NodePtr node = nullptr;
private:
	Tree * tree;
};

template<class Tree>
class ConstTreeIterator : public TreeIterator<Tree>
{
	using NodePtr = typename Tree::Node*;
	using pointer = typename Tree::Node::data const;
	using const_reference = typename Tree::reference const;

public:

	ConstTreeIterator() : TreeIterator<Tree>(NodePtr node, Tree *tree) {}

	pointer operator->() const
	{
		return std::pointer_traits<pointer>::pointer_to(**this);
	}

	const_reference operator*() const
	{
		return node->data;
	}
};

template<class Type, class Compare = std::less<Type>, class Allocator = std::allocator<Type>> // TODO: Sepperate functionality into sepperate classes
class RedBlackTree
{
	using RbTree = RedBlackTree<Type, Compare, Allocator>;

	using reference = Type&;	

	enum Color { RED, BLACK };
	enum Direction { LEFT, RIGHT };

	struct Node
	{
		Node* parent;
		Node* subtree[2];

		Color color; // change this to char when done debugging
		Type data;
	};

	Compare compare;

	using NodeAl = RebindAllocator<Allocator, Node>;				
	using NodeAlTraits = std::allocator_traits<NodeAl>;

	NodeAl nodeAl;
	NodeAlTraits nodeAlTraits;

	// TODO: Add support for <key, type> containers, possibly with a bool template param?

	using Iterator = TreeIterator<RbTree>;
	using reverse_iterator = std::reverse_iterator<Iterator>;
	using Const_Iterator = ConstTreeIterator<RbTree>;
	friend class Iterator;

public:

	RedBlackTree()
	{
		createHead();
	}

private: 
	Node * Head; // TODO: Make head node into parent of root and all null nodes once done testing
	Node * root = nullptr;
	long treeSize = 0;

public:

	Iterator begin() noexcept { return Iterator(lMost(), this); } // Replace lMost() with a cached left value instead of looking it up!!
	Iterator end() noexcept { return Iterator(Head, this); }

	Const_Iterator cbegin() noexcept { return Const_Iterator(lMost(), this); }
	Const_Iterator cend() noexcept { return Const_Iterator(Head, this); }

	reverse_iterator rbegin() noexcept { return reverse_iterator(rMost(), this); }
	reverse_iterator rend() noexcept { return reverse_iterator(lMost(), this); }

private:

	void createHead()
	{
		Node* n = nodeAl.allocate(1);
		NodeAlTraits::construct(nodeAl, std::addressof(n->subtree[0]), nullptr);
		NodeAlTraits::construct(nodeAl, std::addressof(n->subtree[1]), nullptr); // TODO: Try - Catch
		NodeAlTraits::construct(nodeAl, std::addressof(n->parent    ), nullptr);
		
		n->color = BLACK;
		Head = n; // Heads right needs to be rmost, left needs to be lmost
	}

	Node * createNode(Type &&t) // Argument forwarding needed for more complex types, also, template?
	{
		Node* n = nodeAl.allocate(1); // Should allocater be instantiated here?
		NodeAlTraits::construct(nodeAl, std::addressof(n->subtree[0]), nullptr); // Are these needed?
		NodeAlTraits::construct(nodeAl, std::addressof(n->subtree[1]), nullptr); // TODO: Try - Catch
		NodeAlTraits::construct(nodeAl, std::addressof(n->parent    ), nullptr);
		NodeAlTraits::construct(nodeAl, std::addressof(n->data      ),    t);

		n->color = RED;

		return n;
	}
public:
	void emplace(Type &&t)
	{
		addNode(std::move(t));
	}

	void emplace(const Type& t) // Return iterator to elem?
	{
		addNode(t);
	}

	// TODO: topDownInsetion(Type &&T)
private:

	void addNode(Type &&t) // Add exception handling?
	{
		if (root == nullptr)
		{
			root = createNode(std::move(t));
			Head->parent = root;
		}
		else
		{
			int dir;
			Node* c = root;

			for (;;)
			{
				dir = compare(c->data, t);

				if (c->data == t) // Don't overwrite here
					return;
				// Break when leaf node is found in comparative direction
				else if (c->subtree[dir] == nullptr)
					break;

				c = c->subtree[dir]; // Navigate down tree
			}

			c->subtree[dir] = createNode(std::move(t));
			c->subtree[dir]->parent = c;						// Think about caching leftmost and rightmost here with if statements?

			bottomUpInsertion(c->subtree[dir]);
			// TODO: Add in top down insertion and test benifits
		}

		root->color = BLACK;
		++treeSize;

		//testTree(root);
	}

	void bottomUpInsertion(Node *child)
	{
		if (!child->parent)
			return;

		Node* root = this->root;

		// We need to fix tree for children with red parents
		for (Node *c = child; c->parent->color == RED;)
		{
			const Direction dir = static_cast<Direction>(c->parent == c->parent->parent->subtree[RIGHT]); // Change to int once done debugging

			// If sister node is red attempt a re-color
			Node* uncle = c->parent->parent->subtree[!dir];

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

public:
	
	void erase(const Type& t) // Return iterators to next element
	{
		auto td = find(t);

		if(td != end())
			erase(td);
	}

	void freeNode(Node* n)
	{
		NodeAlTraits::destroy(nodeAl, std::addressof(n->subtree[LEFT]));
		NodeAlTraits::destroy(nodeAl, std::addressof(n->subtree[RIGHT]));
		NodeAlTraits::destroy(nodeAl, std::addressof(n->parent));
		NodeAlTraits::deallocate(nodeAl, n, 1);
	}

	Iterator erase(const Iterator& it)
	{
		Iterator successor = it;
		++successor;

		Node* eraseNode = deleteElement(it);

		NodeAlTraits::destroy(nodeAl, std::addressof(eraseNode));
		freeNode(eraseNode);

		testTree(this->root);

		return Iterator(successor.node, this);
	}

private:

	Node* deleteElement(Iterator it) // change to const iterator
	{
		// Save successor iterator for return
		Node* eraseNode = it.node;
		++it;

		Node* pnode = eraseNode; 
		Node* fixParent;
		Node* fixNode;

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

				else if ((!pnode->subtree[ tdir] || pnode->subtree[ tdir]->color == BLACK)
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

	/*
	// Top Down deletion
	void deleteElement(const Type& t)
	{
		if (this->root)
		{
			Node falseHead;
			Node *p, *c, *g, *f;

			int dir = RIGHT;

			c = &head;
			p = g = f = nullptr;

			c->subtree[dir] = this->root;

			// Search for our node to delete while
			// pushing down a red node
			while (c->subtree[dir])
			{
				int last = dir;

				g = p;
				p = c;
				c = c->subtree[dir];

				dir = compare(c->data, t);

				// We've found the node to delete
				if (c->data == t)
					f = c;

				if (isRed(c) && isRed(c->subtree[!dir]))
				{
					if (isRed(c->subtree[!dir]))
					{
						rotateDir(c, dir);
					}

					else if (!isRed(c->subtree[!dir]))
					{
						auto* s = p->subtree[!last];

						if (s)
						{
							if (isRed(s->subtree[!last]) && !isRed(s->subtree[last]))
							{
								p->color = BLACK;
								s->color = RED;
								c->color = RED;
							}
							else
							{
								int dir2 = g->subtree[RIGHT] == p;

								if (isRed(s->subtree[last]))
								{

								}
							}
						}
					}
				}
			}
		}
	}
	*/
public:

	Iterator find(const Type& t) // Change to const_iterator return
	{
		Node* p = lowerBound(t);

		return Iterator(p, this);
	}

private:

	// Find the left most node not less than t
	Node* lowerBound(const Type& t) const				// Switch to iterators
	{
		Node* n = this->root;
		Node* best = this->Head;

		while (n)
		{
			const int dir = compare(n->data, t);

			if (!dir)
				best = n;

			n = n->subtree[dir];
		}

		return best;
	}

	static Node* minNode(Node* p) 
	{
		while (p->subtree[LEFT])
			p = p->subtree[LEFT];

		return p;
	}

	Node* lMost() const
	{
		Node* p = this->root;

		while (p->subtree[LEFT])
			p = p->subtree[LEFT];

		return p;
	}

	Node* rMost() const
	{
		Node* p = this->root;
		while (p->subtree[RIGHT])
			p = p->subtree[RIGHT];
		return p;
	}

	void rotateDir(Node *root, const int dir)
	{
		Node* newRoot = root->subtree[!dir];
		root->subtree[!dir] = newRoot->subtree[dir];

		if (newRoot->subtree[dir])
			newRoot->subtree[dir]->parent = root;

		newRoot->parent = root->parent;

		if (root == this->root)
			this->root = newRoot;

		else
			root->parent->subtree[ (root == root->parent->subtree[dir]) ? dir : !dir] = newRoot; // This branch can be avoided if we pass a third param, since it's calculated in bottomupinsertion
																							     // Worth it or not?
		newRoot->subtree[dir] = root;
		root->parent = newRoot;
	}

	bool isRed(Node *n)
	{
		return (n && n->color == RED);
	}

	int testTree(Node *root)
	{
		if (!root || root == Head)
			return 1;

		Node* left = root->subtree[LEFT];
		Node* right = root->subtree[RIGHT];

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

// Notes: 
// Root is black
// Node that's colored Red cannot have a child Colored Red			  ~ red violation if not true
// Number of black nodes along any path in the tree must be identical ~ black height of the tree ~ black violation if not  true

// Height cannot be shorter than log(N + 1) and cannot be taller than 2 * Log(N + 1)


// Rotations:
// A single rotation sets the old root to Red and the new root to black
// A double rotation

// Double rotation
// - rotate once in !direction of initial dir on the !dir subtree
// - rotate once on the root in rotation direction

/*
struct jsw_node *jsw_single(struct jsw_node *root, int dir)
{
    struct jsw_node *save = root->link[!dir];

    root->link[!dir] = save->link[dir];
    save->link[dir] = root;

    root->red = 1;
    save->red = 0;

    return save;
}

struct jsw_node *jsw_double(struct jsw_node *root, int dir)
{
    root->link[!dir] = jsw_single(root->link[!dir], !dir);

    return jsw_single(root, dir);
}
*/
