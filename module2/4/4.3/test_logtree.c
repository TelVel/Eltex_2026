#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logtree.h"

static int failed;

static void check(int cond, const char *msg) {
    if (!cond) {
        printf("FAIL: %s\n", msg);
        failed++;
    }
}

static Person *find_id(Person *node, int id) {
    if (node == NULL) return NULL;
    if (id < node->id) return find_id(node->left, id);
    if (id > node->id) return find_id(node->right, id);
    return node;
}

static int count_nodes(Person *node) {
    if (node == NULL) return 0;
    return 1 + count_nodes(node->left) + count_nodes(node->right);
}

static int balanced(Person *node) {
    int diff;
    if (node == NULL) return 1;
    diff = getBalance(node);
    if (diff > 1 || diff < -1) return 0;
    if (node->height != 1 + max(height(node->left), height(node->right))) return 0;
    return balanced(node->left) && balanced(node->right);
}

static void free_tree(Person *node) {
    if (node == NULL) return;
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

static Person *seed(void) {
    Person *root = NULL;
    root = insert(root, 0, "LastName10", "FirstName0", "MiddleName0", "Company0", "0@gmail.com", "0_tg", "0_inst");
    root = insert(root, 55, "LastName51", "FirstName0", "MiddleName0", "Company0", "0@gmail.com", "0_tg", "0_inst");
    root = insert(root, 99, "LastName12", "FirstName0", "MiddleName0", "Company0", "0@gmail.com", "0_tg", "0_inst");
    root = insert(root, 44, "LastName44", "FirstName0", "MiddleName0", "Company0", "0@gmail.com", "0_tg", "0_inst");
    root = insert(root, 88, "LastName33", "FirstName0", "MiddleName0", "Company0", "0@gmail.com", "0_tg", "0_inst");
    root = insert(root, 12, "LastName88", "FirstName0", "MiddleName0", "Company0", "0@gmail.com", "0_tg", "0_inst");
    root = insert(root, 55, "LastName99", "FirstName0", "MiddleName0", "Company0", "0@gmail.com", "0_tg", "0_inst");
    root = insert(root, 19, "LastName66", "FirstName0", "MiddleName0", "Company0", "0@gmail.com", "0_tg", "0_inst");
    root = insert(root, 22, "LastName22", "FirstName0", "MiddleName0", "Company0", "0@gmail.com", "0_tg", "0_inst");
    root = insert(root, 9, "LastName19", "FirstName0", "MiddleName0", "Company0", "0@gmail.com", "0_tg", "0_inst");
    return root;
}

int main(void) {
    Person *root = seed();
    Person *found;

    check(count_nodes(root) == 9, "duplicate id is not inserted");
    check(balanced(root), "seeded tree stays balanced");
    found = find_id(root, 55);
    check(found != NULL && strcmp(found->LastName, "LastName51") == 0, "first insert of an id is kept");
    check(find_id(root, 9) != NULL && find_id(root, 99) != NULL, "both ends are present");
    check(find_id(root, 100) == NULL, "missing id is absent");

    root = deleteNode(root, 0);
    check(find_id(root, 0) == NULL, "delete removes the id");
    check(count_nodes(root) == 8, "delete drops one node");
    check(balanced(root), "tree stays balanced after delete");

    root = insert(root, 100, "New", "Name", "Mid", "Work", "n@n.n", "tg", "ig");
    found = find_id(root, 100);
    check(found != NULL && strcmp(found->Social.EmailAddress, "n@n.n") == 0, "insert stores the new contact");
    check(balanced(root), "tree stays balanced after insert");

    root = deleteNode(root, 404);
    check(count_nodes(root) == 9, "missing delete does nothing");

    preOrder(root);
    printTree(root, 0);
    free_tree(root);

    if (failed) {
        printf("%d test(s) failed\n", failed);
        return 1;
    }
    printf("all tests passed\n");
    return 0;
}
