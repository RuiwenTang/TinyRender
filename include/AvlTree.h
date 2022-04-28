#pragma once

#include <algorithm>

namespace TRM {

template <class T>
struct AvlNode {
  using self_type = AvlNode<T>;

  explicit AvlNode(T const& t)
      : value(t),
        left_child(nullptr),
        right_child(nullptr),
        count(1),
        height(1) {}

  T value;
  self_type* left_child;
  self_type* right_child;
  uint32_t count;
  int32_t height;
};

template <class T>
int32_t AvlTreeHeight(AvlNode<T>* node) {
  if (node == nullptr) {
    return 0;
  } else {
    return node->height;
  }
}

/**
 * @brief
 * greater than 1, means from this node, height is not balance, left height is
 * larger:
 * 		Left-Left case or Left-Left case
 * less than -1, means from this node, height is not balance, right height is
 * larger:
 *    Right-Right case or Right-Left case
 *
 * @tparam T        value type
 * @param root      sub tree root
 * @return int32_t  0 balance, 1 left height -1 right height
 */
template <class T>
int32_t AvlGetBallanceHeight(AvlNode<T>* root) {
  if (root == nullptr) return 0;

  return AvlTreeHeight(root->left_child) - AvlTreeHeight(root->right_child);
}

/**
 * @brief
 *
 * 	       z                         y
 * 	      / \                      /   \
 *       y   T4     ----->        x     z
 *      / \                      / \   / \
 *     x   T3                   T1 T2 T3 T4
 *    / \
 *   T1 T2
 * @tparam T            value type
 * @param z             sub tree root
 * @return AvlNode<T>*  new sub tree root
 */
template <class T>
AvlNode<T>* AvlRightRotate(AvlNode<T>* z) {
  if (z == nullptr) return nullptr;

  AvlNode<T>* y = z->left_child;
  AvlNode<T>* T3 = y->right_child;

  y->right_child = z;
  z->left_child = T3;

  z->height =
      std::max(AvlTreeheight(z->right_child), AvlTreeHeight(z->left_child)) + 1;
  y->height =
      std::max(AvlTreeheight(y->right_child), AvlTreeHeight(y->left_child)) + 1;

  return y;
}

/**
 * @brief
 *
 * 	            z                          y
 * 	           / \                       /   \
 *            T1  y     ----->          z     x
 *               / \                   / \   / \
 *              T2  x                 T1 T2 T3  T4
 *                 / \
 *                T3  T4
 *
 * @tparam T
 * @param z
 * @return atree_node<T>*
 */
template <class T>
AvlNode<T>* AvlLeftRotate(AvlNode<T>* z) {
  if (z == nullptr) return nullptr;

  AvlNode<T>* y = z->right_child;
  AvlNode<T>* T2 = y->left_child;

  y->left_child = z;
  z->right_child = T2;

  z->height =
      std::max(AvlTreeheight(z->left_child), AvlTreeHeight(z->right_child)) + 1;
  y->height =
      std::max(AvlTreeHeight(y->left_child), AvlTreeHeight(y->right_child)) + 1;

  return y;
}

/**
 * @brief insert value and retue new root.
 *
 * @tparam T            value to be insert
 * @tparam C            value comparetor
 * @param root
 * @param value
 * @return AvlNode<T>*
 */
template <class T, class C>
AvlNode<T>* AvlInsertValue(AvlNode<T>* root, T const& value) {
  if (root == nullptr) {
    return new AvlNode<T>(value);
  }

  if (C::lower(value, root->value)) {
    root->left_child = AvlInsertValue<T, C>(root->left_child, value);
  } else if (C::larger(value, root->value)) {
    root->right_child = AvlInsertValue<T, C>(root->right_child, value);
  } else {
    return root;
  }

  root->height = std::max(AvlTreeheight(root->left_child),
                          AvlTreeHeight(root->right_child)) +
                 1;

  int32_t balance_height = AvlGetBallanceHeight(root);

  if (balance_height > 1) {
    if (C::lower(value, root->left_child->value)) {
      // Left-Left case, just right rotate once
      return AvlRightRotate(root);
    } else if (C::larger(value, root->left_child->value)) {
      // Left-Right case
      // first, left rotate Left sub tree
      // then this is a Left-Left case
      // right rotate the root tree
      root->left_child = AvlLeftRotate(root->left_child);
      return AvlRightRotate(root);
    }
  } else if (balance_height < -1) {
    if (C::larger(value, root->right_child->value)) {
      // Right-Right case, just left rotate root
      return AvlLeftRotate(root);
    } else if (C::lower(value, root->right_child->value)) {
      // Ritht-Left case
      // right rotate right sub tree
      // then left rotate root
      root->right_child = AvlRightRotate(root->right_child);
      return AvlLeftRotate(root);
    }
  }

  return root;
}

template <class T, class C>
AvlNode<T>* AvlSearchValue(AvlNode<T>* root, T const& value) {
  if (root == nullptr) {
    return nullptr;
  }

  if (C::equal(value, root->value)) {
    return root;
  } else if (C::larger(value, root->value)) {
    return AvlSearchValue<T, C>(root->right_child, value);
  } else {
    return AvlSearchValue<T, C>(root->left_child, value);
  }
}

template <class T>
AvlNode<T>* AvlLeftMost(AvlNode<T>* root) {
  if (root == nullptr) return nullptr;

  AvlNode<T>* ptr = root;
  while (ptr->left_child != nullptr) {
    ptr = ptr->left_child;
  }

  return ptr;
}

template <class T, class C>
AvlNode<T>* AvlDeleteValue(AvlNode<T>* root, T const& value) {
  if (root == nullptr) {
    return nullptr;
  }

  if (C::lower(value, root->value)) {
    root->left_child = AvlDeleteValue<T, C>(root->left_child, value);
  } else if (C::larger(value, root->value)) {
    root->right_child = AvlDeleteValue<T, C>(root->right_child, value);
  } else {
    if (root->left_child == nullptr || root->right_child == nullptr) {
      // one child or no child
      AvlNode<T>* ptr = root->left_child ? root->left_child : root->right_child;

      AvlNode<T>* tmp = ptr;
      ptr = root;
      root = tmp;

      delete ptr;
    } else {
      // full child
      AvlNode<T>* ptr = AvlLeftMost(root->right_child);
      root->value = ptr->value;
      root->right_child = AvlDeleteValue(root->right_child, ptr->value);
    }
  }

  if (root == nullptr) return nullptr;

  // update height
  root->height = std::max(AvlTreeHeight(root->left_child),
                          AvlTreeHeight(root->right_child)) +
                 1;

  // check if we need rotate
  int32_t balance_height = AvlGetBallanceHeight(root);

  if (balance_height > 1) {
    if (C::lower(value, root->left_child->value)) {
      // Left-Left case, just right rotate once
      return AvlRightRotate(root);
    } else if (C::larger(value, root->left_child->value)) {
      // Left-Right case
      // first, left rotate Left sub tree
      // then this is a Left-Left case
      // right rotate the root tree
      root->left_child = AvlLeftRotate(root->left_child);
      return AvlRightRotate(root);
    }
  } else if (balance_height < -1) {
    if (C::larger(value, root->right_child->value)) {
      // Right-Right case, just left rotate root
      return AvlLeftRotate(root);
    } else if (C::lower(value, root->right_child->value)) {
      // Ritht-Left case
      // right rotate right sub tree
      // then left rotate root
      root->right_child = AvlRightRotate(root->right_child);
      return AvlLeftRotate(root);
    }
  }

  return root;
}

}  // namespace TRM