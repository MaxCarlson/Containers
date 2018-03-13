#pragma once
#include <functional>
#include <memory>


template<class Type, class Compare = std::less<Type>, class Allocator = std::allocator<Type>>
class RedBlackTree
{
	using RbTree = RedBlackTree<Type, Compare, Allocator>;

	Compare compare;
	Allocator alloc;

//public:

	enum Color { RED, BLACK };
	enum Direction { LEFT, RIGHT };

	struct Node
	{
		Node* parent;
		Node* subtree[2];

		Color color; // change this to char when done debugging
		Type data;
	};

	

	long treeSize = 0;

	using NodeAl = std::allocator<Node>;
	//using NodeAl = Allocator::rebind<Node>; // FIX LATER
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

	Node * Head;
	Node * root = nullptr;


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
		Node* n = nodeAl.allocate(1);
		NodeAlTraits::construct(nodeAl, std::addressof(n->subtree[0]), Head);
		NodeAlTraits::construct(nodeAl, std::addressof(n->subtree[1]), Head); // TODO: Try - Catch
		NodeAlTraits::construct(nodeAl, std::addressof(n->parent    ), Head);
		NodeAlTraits::construct(nodeAl, std::addressof(n->data      ),    t);

		n->subtree[0] = n->subtree[1] = n->parent = nullptr; // DELETE THIS ONCE DONE TESTING

		n->color = RED;

		return n;
	}
public:
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

			testRedViolation(c->subtree[dir]);
		}

		root->color = BLACK;
		++treeSize;
	}

	void emplace(Type &&t)
	{
		addNode(t);
	}

	void emplace(const Type& t)
	{
		addNode(t);
	}

	void testRedViolation(Node *child) // TODO: Need to test for null nodes? Probably not once head is implemented as node children and parents for null? Unsure
	{
		if (!child->parent)
			return;

		Node* root = this->root;

		// We need to fix tree for children with red parents
		for (Node *c = child; c->parent->color == RED;)
		{
			//const int dir = c->parent == c->parent->parent->subtree[RIGHT]; // parent dir is 1 if parent is left child of its parent
			//const int dir  = c == c->parent->subtree[RIGHT];				   // dir is 1 if child is right child of parent

			const Direction dir = static_cast<Direction>(c->parent == c->parent->parent->subtree[RIGHT]);

			// If sister node is red attempt a re-color
			Node* uncle = c->parent->parent->subtree[!dir];

			if (uncle && uncle->color == RED)
			{
				c->color = uncle->color = BLACK; // Color parent and it's uncle node black
				c->parent->parent->color = RED;  // Color grandparent red
				c = c->parent->parent;			 // Set current node to grandparent
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

	/*
	_Nodeptr _Pnode = _Wherenode->_Right; // L ROTATE
	_Wherenode->_Right = _Pnode->_Left;

	if (!_Pnode->_Left->_Isnil)
	{
		_Pnode->_Left->_Parent = _Wherenode;
	}

	_Pnode->_Parent = _Wherenode->_Parent;

	if (_Wherenode == _Root())
	{
		_Root() = _Pnode;
	}
	else if (_Wherenode == _Wherenode->_Parent->_Left)
	{
		_Wherenode->_Parent->_Left = _Pnode;
	}
	else
	{
		_Wherenode->_Parent->_Right = _Pnode;
	}

	_Pnode->_Left = _Wherenode;
	_Wherenode->_Parent = _Pnode;
	*/

	void rotateDir(Node *root, const int dir)
	{
		Node* newRoot = root->subtree[!dir];
		root->subtree[!dir] = newRoot->subtree[dir];

		if (newRoot->subtree[dir])
			newRoot->subtree[dir]->parent = root;

		newRoot->parent = root->parent;

		if (root == this->root)
			this->root = newRoot;

		else if (root == root->parent->subtree[dir])
			root->parent->subtree[dir] = newRoot;

		else
			root->parent->subtree[!dir] = newRoot;

		newRoot->subtree[dir] = root;
		root->parent = newRoot;
	}
	
	template<Direction dir>
	Node *doubleRotation(Node *root)
	{
		// Rotate opposite direction on opposite of dir's subtree
		root->subtree[!dir] = rotate<!dir>(root->subtree[!dir]);

		return rotate<dir>(root);
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


// Balance code from compiler
/*
// insert new node.
// 
// if parent of parent has two red children, blacken both
// set parent of parent to red
// 
// else
// dir = newNode->parent == newNode->parent->color == RED;
// parent has red and black children
//
// if(newNode is the right child)
//		rotate

for (_Nodeptr _Pnode = _Newnode; _Pnode->_Parent->_Color == this->_Red; )
	{
	if (_Pnode->_Parent == _Pnode->_Parent->_Parent->_Left)
		{	// fixup red-red in left subtree
		_Wherenode = _Pnode->_Parent->_Parent->_Right;
		if (_Wherenode->_Color == this->_Red)
			{	// parent has two red children, blacken both
			_Pnode->_Parent->_Color = this->_Black;
			_Wherenode->_Color = this->_Black;
			_Pnode->_Parent->_Parent->_Color = this->_Red;
			_Pnode = _Pnode->_Parent->_Parent;
			}
		else
			{	// parent has red and black children
			if (_Pnode == _Pnode->_Parent->_Right)
				{	// rotate right child to left
				_Pnode = _Pnode->_Parent;
				_Lrotate(_Pnode);
				}

			_Pnode->_Parent->_Color = this->_Black;	// propagate red up
			_Pnode->_Parent->_Parent->_Color = this->_Red;
			_Rrotate(_Pnode->_Parent->_Parent);
			}
		}
	else
		{	// fixup red-red in right subtree
		_Wherenode = _Pnode->_Parent->_Parent->_Left;
		if (_Wherenode->_Color == this->_Red)
			{	// parent has two red children, blacken both
			_Pnode->_Parent->_Color = this->_Black;
			_Wherenode->_Color = this->_Black;
			_Pnode->_Parent->_Parent->_Color = this->_Red;
			_Pnode = _Pnode->_Parent->_Parent;
			}
		else
			{	// parent has red and black children
			if (_Pnode == _Pnode->_Parent->_Left)
				{	// rotate left child to right
				_Pnode = _Pnode->_Parent;
				_Rrotate(_Pnode);
				}

			_Pnode->_Parent->_Color = this->_Black;	// propagate red up
			_Pnode->_Parent->_Parent->_Color = this->_Red;
			_Lrotate(_Pnode->_Parent->_Parent);
			}
		}
	}

*/