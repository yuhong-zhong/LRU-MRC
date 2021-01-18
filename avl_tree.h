#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <memory>
#include <algorithm>

using namespace std;

template<typename K, typename V>
class avl_tree {
private:
	class avl_tree_node {
	public:
		K key;
		V value;
		shared_ptr<avl_tree_node> left, right, parent;
		long height;
		long num_nodes;
		V sum_value;

		avl_tree_node(K key, V value, shared_ptr<avl_tree_node> left, shared_ptr<avl_tree_node> right,
		              shared_ptr<avl_tree_node> parent, long height, long num_nodes, V sum_value)
			: key(key), value(value), left(left), right(right), parent(parent),
			  height(height), num_nodes(num_nodes), sum_value(sum_value) {
		};
	};

	shared_ptr<avl_tree_node> root = nullptr;
	long num_nodes = 0;
	V sum_value = 0;

	static void update_sum_value(shared_ptr<avl_tree_node> node) {
		V sum_value = node->value;
		if (node->left != nullptr) {
			sum_value += node->left->sum_value;
		}
		if (node->right != nullptr) {
			sum_value += node->right->sum_value;
		}

		node->sum_value = sum_value;
	}

	static void update_num_nodes(shared_ptr<avl_tree_node> node) {
		long num_nodes = 1;
		if (node->left != nullptr) {
			num_nodes += node->left->num_nodes;
		}
		if (node->right != nullptr) {
			num_nodes += node->right->num_nodes;
		}

		node->num_nodes = num_nodes;
	}

	static void update_height(shared_ptr<avl_tree_node> node) {
		long children_height = 0;
		if (node->left != nullptr) {
			children_height = max(children_height, node->left->height);
		}
		if (node->right != nullptr) {
			children_height = max(children_height, node->right->height);
		}

		node->height = children_height + 1;
	}

	static void update_meta(shared_ptr<avl_tree_node> node) {
		update_height(node);
		update_num_nodes(node);
		update_sum_value(node);
	}

	static void update_meta_recursive(shared_ptr<avl_tree_node> node) {
		while (node != nullptr) {
			update_meta(node);
			node = node->parent;
		}
	}

	static void swap_node(shared_ptr<avl_tree_node> node_1, shared_ptr<avl_tree_node> node_2) {
		/* update value */
		V value_1 = node_1->value;
		node_1->value = node_2->value;
		update_meta_recursive(node_1);
		node_2->value = value_1;
		update_meta_recursive(node_2);

		/* update key */
		K key_1 = node_1->key;

		node_1->key = node_2->key;
		node_2->key = key_1;
	}

	static long get_balance_factor(shared_ptr<avl_tree_node> node) {
		long left_height = 0;
		if (node->left != nullptr) {
			left_height = node->left->height;
		}
		long right_height = 0;
		if (node->right != nullptr) {
			right_height = node->right->height;
		}
		return left_height - right_height;
	}

	void right_rotate(shared_ptr<avl_tree_node> node) {
		shared_ptr<avl_tree_node> left = node->left;
		if (node->parent != nullptr) {
			if (node == node->parent->left) {
				node->parent->left = left;
			} else {
				node->parent->right = left;
			}
		} else {
			root = left;
		}
		left->parent = node->parent;

		node->left = left->right;
		if (node->left != nullptr) {
			node->left->parent = node;
		}

		left->right = node;
		node->parent = left;

		update_meta(node);
		update_meta(left);
	}

	void left_rotate(shared_ptr<avl_tree_node> node) {
		shared_ptr<avl_tree_node> right = node->right;
		if (node->parent != nullptr) {
			if (node == node->parent->left) {
				node->parent->left = right;
			} else {
				node->parent->right = right;
			}
		} else {
			root = right;
		}
		right->parent = node->parent;

		node->right = right->left;
		if (node->right != nullptr) {
			node->right->parent = node;
		}

		right->left = node;
		node->parent = right;

		update_meta(node);
		update_meta(right);
	}

	void balance(shared_ptr<avl_tree_node> node) {
		while (node != nullptr) {
			update_meta(node);

			shared_ptr<avl_tree_node> parent = node->parent;
			long balance_factor = get_balance_factor(node);
			if (balance_factor < -1) {
				long right_balance_factor = get_balance_factor(node->right);
				if (right_balance_factor == -1 || right_balance_factor == 0) {
					left_rotate(node);
				} else {
					right_rotate(node->right);
					left_rotate(node);
				}
			} else if (balance_factor > 1) {
				long left_balance_factor = get_balance_factor(node->left);
				if (left_balance_factor == 0 || left_balance_factor == 1) {
					right_rotate(node);
				} else {
					left_rotate(node->left);
					right_rotate(node);
				}
			}
			node = parent;
		}
	}

public:
	void insert(K key, V value) {
		if (root == nullptr) {
			root = make_shared<avl_tree_node>(key, value, nullptr, nullptr, nullptr, 1, 1, value);
			++num_nodes;
			sum_value += value;
			return;
		}

		shared_ptr<avl_tree_node> cur_node = root;
		while (true) {
			if (key > cur_node->key) {
				if (cur_node->right != nullptr) {
					cur_node = cur_node->right;
				} else {
					cur_node->right = make_shared<avl_tree_node>(key, value, nullptr, nullptr, cur_node,
					                                             1, 1, value);
					break;
				}
			} else {
				if (cur_node->left != nullptr) {
					cur_node = cur_node->left;
				} else {
					cur_node->left = make_shared<avl_tree_node>(key, value, nullptr, nullptr, cur_node,
					                                            1, 1, value);
					break;
				}
			}
		}
		++num_nodes;
		sum_value += value;

		/* re-balance & update meta data */
		balance(cur_node);
	}

	void remove(K key) {
		/* search for the target node */
		shared_ptr<avl_tree_node> cur_node = root;
		while (cur_node != nullptr) {
			if (key == cur_node->key) {
				break;
			} else if (key < cur_node->key) {
				cur_node = cur_node->left;
			} else {
				cur_node = cur_node->right;
			}
		}
		if (cur_node == nullptr) {
			return;
		}

		/* if the target node has both left and right child, swap it with its precedent */
		if (cur_node->left != nullptr && cur_node->right != nullptr) {
			shared_ptr<avl_tree_node> precedent = cur_node->left;
			while (precedent->right != nullptr) {
				precedent = precedent->right;
			}
			swap_node(cur_node, precedent);
			cur_node = precedent;
		}

		/* find the new subtree root after removing the target node */
		shared_ptr<avl_tree_node> new_subtree_root;
		if (cur_node->left != nullptr) {
			new_subtree_root = cur_node->left;
		} else if (cur_node->right != nullptr) {
			new_subtree_root = cur_node->right;
		} else {
			new_subtree_root = nullptr;
		}

		/* remove target node from the tree */
		if (cur_node->parent == nullptr) {
			root = new_subtree_root;
		} else if (cur_node == cur_node->parent->left) {
			cur_node->parent->left = new_subtree_root;
		} else {
			cur_node->parent->right = new_subtree_root;
		}

		if (new_subtree_root != nullptr) {
			new_subtree_root->parent = cur_node->parent;
		}

		--num_nodes;
		sum_value -= cur_node->value;

		/* re-balance & update meta data */
		balance(cur_node->parent);
	}

	bool contains(K key) {
		/* search for the target node */
		shared_ptr<avl_tree_node> cur_node = root;
		while (cur_node != nullptr) {
			if (key == cur_node->key) {
				return true;
			} else if (key < cur_node->key) {
				cur_node = cur_node->left;
			} else {
				cur_node = cur_node->right;
			}
		}
		return false;
	}

	V sum_value_less(K key, bool with_equal) {
		V sum = 0;

		shared_ptr<avl_tree_node> cur_node = root;
		while (cur_node != nullptr) {
			if (key == cur_node->key) {
				if (with_equal) {
					sum += cur_node->value + ((cur_node->left != nullptr) ? cur_node->left->sum_value : 0);
					break;
				} else {
					cur_node = cur_node->left;
				}
			} else if (key > cur_node->key) {
				sum += cur_node->value + ((cur_node->left != nullptr) ? cur_node->left->sum_value : 0);
				cur_node = cur_node->right;
			} else {
				cur_node = cur_node->left;
			}
		}

		return sum;
	}

	V sum_value_greater(K key, bool with_equal) {
		return sum_value - sum_value_less(key, !with_equal);
	}

	V sum_value_between(K key_1, K key_2, bool left_inclusive, bool right_inclusive) {
		return sum_value_less(key_2, right_inclusive) - sum_value_less(key_1, !left_inclusive);
	}

	long count_less(K key, bool with_equal) {
		long counter = 0;

		shared_ptr<avl_tree_node> cur_node = root;
		while (cur_node != nullptr) {
			if (key == cur_node->key) {
				if (with_equal) {
					counter += 1 + ((cur_node->left != nullptr) ? cur_node->left->num_nodes : 0);
					break;
				} else {
					cur_node = cur_node->left;
				}
			} else if (key > cur_node->key) {
				counter += 1 + ((cur_node->left != nullptr) ? cur_node->left->num_nodes : 0);
				cur_node = cur_node->right;
			} else {
				cur_node = cur_node->left;
			}
		}

		return counter;
	}

	long count_greater(K key, bool with_equal) {
		return num_nodes - count_less(key, !with_equal);
	}

	long size() {
		return num_nodes;
	}

	float percent_less(K key, bool with_equal) {
		if (num_nodes == 0) {
			return numeric_limits<float>::quiet_NaN();
		}

		return (float) count_less(key, with_equal) / (float) num_nodes;
	}

	float percent_greater(K key, bool with_equal) {
		return 1.0f - percent_less(key, !with_equal);
	}

	class iterator {
	private:
		void find_next() {
			if (cur_node == nullptr) {
				next_node = nullptr;
				return;
			}

			if (cur_node->right != nullptr) {
				next_node = cur_node->right;
				while (next_node->left != nullptr) {
					next_node = next_node->left;
				}
			} else {
				shared_ptr<avl_tree_node> prev_node = cur_node;
				next_node = prev_node->parent;
				while (next_node != nullptr && prev_node == next_node->right) {
					prev_node = next_node;
					next_node = next_node->parent;
				}
			}
		}

		void init() {
			cur_node = root;
			next_node = nullptr;

			if (cur_node == nullptr) {
				return;
			}
			while (cur_node->left != nullptr) {
				cur_node = cur_node->left;
			}
			find_next();
		}

	public:
		iterator(avl_tree<K, V> &tree) {
			root = tree.root;
			init();
		}

		pair<K, V> operator*() {
			return make_pair(cur_node->key, cur_node->value);
		}

		iterator operator++() {
			cur_node = next_node;
			find_next();
			return *this;
		}

		iterator operator++(int _) {
			iterator cur = *this;
			cur_node = next_node;
			find_next();
			return cur;
		}

		explicit operator bool() {
			return cur_node != nullptr;
		}

	private:
		shared_ptr<avl_tree_node> root;
		shared_ptr<avl_tree_node> cur_node;
		shared_ptr<avl_tree_node> next_node;
	};
};

#endif  // AVL_TREE_H
