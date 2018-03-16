#pragma once
#include <functional>
#include <memory>

template<class Alloc, class Type>
using RebindAllocator = typename std::allocator_traits<Alloc>::template rebind_alloc<Type>; // Move this into a traits file for containers?

template<class Type, class Compare = std::less<Type>, class Allocator = std::allocator<Type>> // TODO: Sepperate functionality into sepperate classes
class RedBlackTree
{
	using RbTree = RedBlackTree<Type, Compare, Allocator>;

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

	using NodeAl = RebindAllocator<Allocator, Node>;				//using NodeAl = Allocator::rebind<Node>; // FIX LATER
	using NodeAlTraits = std::allocator_traits<NodeAl>;

	NodeAl nodeAl;
	NodeAlTraits nodeAlTraits;

	// TODO: Add support for <key, type> containers

public:

	RedBlackTree()
	{
		createHead();
	}

private: 

	Node * Head; // TODO: Make head node into parent of root and all null nodes once done testing
	Node * root = nullptr;
	long treeSize = 0;

	struct Iterator // Possibly move this outside BinarySearchTree and use it as a general case for any BST based structures Iterator?
	{
		Iterator() = default;
		Iterator(Node *n, RbTree *t) : it(n), tree(t)
		{
			data = &n->data;
		}

		bool operator!=(const Iterator& i) const
		{
			return data != i.data;
		}

		bool operator==(const Iterator& i) const
		{
			return data == i.data; // Compare ptr's not data itself
		}

		const Type & operator*()
		{
			return *data;
		}

		Iterator& operator++() // Preincrement
		{
			if (!data) // Don't increment on null node, usually end()
				;
			else if (it->subTree[1]) // Right node is non empty, find smallest member
			{
				it = minNode(it->subTree[1]);
				data = &it->data;
			}
			else // Climb up tree looking for first Node with a non-empty right subtree
			{
				Node* n;
				// While node has a parent and the iterator is equal
				// to its parents right node, traverse up tree
				while ((n = it->parent) && it == n->subTree[1])
					it = n;

				bool found = false;
				if (n)
				{
					it = n;
					data = &it->data;
					found = true;
				}

				// Inelegant way to set Iterator to end
				if (!found)
					*this = tree->Head; // How to replace?
			}
			return *this;
		}

		Iterator operator++(int) // Postincrement
		{
			Iterator i = *this;
			++*this;
			return i;
		}

		Iterator& operator--()
		{
			if (!data)
				;
			else if (it->subTree[0]) // Left node is non empty, find largest memeber
			{
				it = maxNode(it->subTree[0]);
				data = &it->data;
			}
			else
			{
				Node *n;
				while ((n = it->parent) && it == n->subTree[0])
					it = n;

				if (n)
				{
					it = n;
					data = &it->data;
				}
				else
					*this = tree->Head;

			}
			return *this;
		}

		Iterator operator--(int)
		{
			Iterator i = *this;
			--*this;
			return i;
		}

	protected:
		friend class RbTree;
		Node *it;
		Type *data; // Get rid of this and allocate node data sepperatly from nodes?
		RbTree * tree; // How to get rid of this? Need it for head currently
	};

public:

	Iterator begin() { return Iterator(); }

private:

	void createHead()
	{
		Node* n = nodeAl.allocate(1);
		NodeAlTraits::construct(nodeAl, std::addressof(n->subtree[0]), n);
		NodeAlTraits::construct(nodeAl, std::addressof(n->subtree[1]), n); // TODO: Try - Catch
		NodeAlTraits::construct(nodeAl, std::addressof(n->parent    ), n);

		//n->subtree[0] = n->subtree[1] = n->parent = nullptr; // DELETE THIS ONCE DONE TESTING
		
		n->color = BLACK;
		Head = n;
	}

	Node * createNode(Type &&t) // Argument forwarding needed for more complex types, also, template?
	{
		Node* n = nodeAl.allocate(1); // Should allocater be instantiated here?
		NodeAlTraits::construct(nodeAl, std::addressof(n->subtree[0]), Head);
		NodeAlTraits::construct(nodeAl, std::addressof(n->subtree[1]), Head); // TODO: Try - Catch
		NodeAlTraits::construct(nodeAl, std::addressof(n->parent    ), Head);
		NodeAlTraits::construct(nodeAl, std::addressof(n->data      ),    t);

		n->subtree[0] = n->subtree[1] = n->parent = nullptr; // DELETE THIS ONCE DONE TESTING

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
			root = createNode(std::move(t));
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
		deleteElement(t);
	}

private:

	void destoryNode(Node *n)
	{
		nodeAl.destroy(n);
	}

	void deleteExternal(Node* p, Node *c)
	{

	}

	void deleteElement(const Type& t)
	{
		// Find node to delete
		auto nodeIt = find(t);
		// Save successor iterator for return
		// const_iterator returnVal = ++nodeIt;

		if (!nodeIt)
			return;

		Node* pnode = nodeIt; // Will be *nodeIt once iterators are implemented
		Node* eraseNode = pnode;
		Node* fixParent;
		Node* fixNode;

		if (!pnode->subtree[LEFT])
			fixNode = pnode->subtree[LEFT];

		else if (!pnode->subtree[RIGHT])
			fixNode = pnode->subtree[RIGHT];

		else
		{
			//node = successor
			fixNode = pnode->subtree[RIGHT];
		}

		// Only one subtree
		if (pnode == eraseNode)
		{
			fixParent = eraseNode->parent;

			if (fixNode)
				fixNode->parent = fixParent;

			if (this->root == eraseNode)
				this->root = fixNode;
			else
			{
				int tdir = fixParent->subtree[RIGHT] == eraseNode;
				fixParent->subtree[tdir] = fixNode;
			}

			// Add left most and right most caching here
		}
		else
		{
			eraseNode->subtree[LEFT]->parent = pnode;
			pnode->subtree[LEFT] = eraseNode->subtree[LEFT];

			if (pnode == eraseNode->subtree[LEFT])
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

			if (eraseNode->color == BLACK) // Have to recolor tree when erasing non-red parent || child
			{
				for (; fixNode != this->root && fixNode->color == BLACK; fixParent = fixNode->parent)
				{
					int tdir = fixNode == fixParent->subtree[RIGHT];

					// Fixup tdir subtree
					pnode = fixParent->[!tdir]; // tdir is LEFT or 0 for first case!!

					if (pnode->color == RED) // rotate red up from !tdir subtree
					{
						pnode->color = BLACK;
						fixParent->color = RED;
						rotateDir(fixParent, tdir);
						pnode = fixParent->subtree[!tdir];
					}

					if (!pnode)
						fixNode = fixParent;

					else if (pnode->subtree[ tdir]->color == BLACK
						  && pnode->subtree[!tdir]->color == BLACK) // Redden right subtree that has two black children
					{
						pnode->color = RED;
						fixNode = fixParent;
					}
					else
					{ // Rearrange !tdir subtree
						if (pnode->subtree[!tdir]->color == BLACK)
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

				fixNode->color = BLACK;
			}

			if (treeSize > 0)
				--treeSize;


			return eraseNode;
		}
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

	Type* find(const Type& t) // Change to iterator return
	{
		Node* p = lowerBound(t);

		Type *r = nullptr;
		if (p)
			r = &p->data;

		return r;
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
		if (!root)
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
