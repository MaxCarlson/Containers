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

	void createHead()
	{
		Node* n = nodeAl.allocate(1);
		NodeAlTraits::construct(nodeAl, std::addressof(n->subtree[0]), n);
		NodeAlTraits::construct(nodeAl, std::addressof(n->subtree[1]), n); // TODO: Try - Catch
		NodeAlTraits::construct(nodeAl, std::addressof(n->parent), n);

		n->subtree[0] = n->subtree[1] = n->parent = nullptr; // DELETE THIS ONCE DONE TESTING
		
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

		Node* node = nodeIt; // Will be *nodeIt once iterators are implemented
		Node* Parent = nodeIt->parent;

		int dir = Parent->subtree[RIGHT] == node;

		Node* eraseNode = node;
		Node* fixNode;


		if (!node->subtree[LEFT])
			fixNode = node->subtree[LEFT];
		else if (!node->subtree[RIGHT])
			fixNode = node->subtree[RIGHT];
		else
		{
			//node = successor
			fixNode = node->subtree[RIGHT];
		}

		// Only one subtree
		if (node == eraseNode)
		{

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
