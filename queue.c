#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list_sort.h"
#include "queue.h"

/* Compare two elements based on their string values. */
static inline int cmp(const struct list_head *a, const struct list_head *b)
{
    const element_t *ela = list_entry(a, element_t, list);
    const element_t *elb = list_entry(b, element_t, list);

    return strcmp(ela->value, elb->value);
}

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head == NULL) {
        return NULL;
    }
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head) {
        return;
    }

    element_t *entry = NULL, *safe = NULL;
    list_for_each_entry_safe(entry, safe, head, list) {
        q_release_element(entry);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s) {
        return false;
    }

    element_t *entry = malloc(sizeof(element_t));

    if (!entry) {
        free(entry);
        return false;
    }

    entry->value = strdup(s);

    if (!entry->value) {
        free(entry);
        return false;
    }

    list_add(&entry->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s) {
        return false;
    }

    element_t *entry = malloc(sizeof(element_t));
    if (!entry) {
        free(entry);
        return false;
    }

    entry->value = strdup(s);

    if (!entry->value) {
        free(entry);
        return false;
    }

    list_add_tail(&entry->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || list_empty(head)) {
        return NULL;
    }

    element_t *entry = list_first_entry(head, element_t, list);
    if (sp && bufsize > 0) {
        strncpy(sp, entry->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(&entry->list);

    return entry;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || list_empty(head)) {
        return NULL;
    }

    element_t *entry = list_last_entry(head, element_t, list);
    if (sp && bufsize > 0) {
        strncpy(sp, entry->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    list_del(&entry->list);

    return entry;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (head == NULL || list_empty(head)) {
        return 0;
    }

    struct list_head *curr;
    int size = 0;

    list_for_each(curr, head) {
        size++;
    }

    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/

    if (head == NULL || list_empty(head)) {
        return false;
    }

    struct list_head *front = head->next;
    struct list_head *tail = head->prev;

    while (front != tail && tail->next != front) {
        front = front->next;
        tail = tail->prev;
    }

    list_del(tail);
    q_release_element(list_entry(tail, element_t, list));

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/

    if (head == NULL || list_empty(head)) {
        return false;
    }

    element_t *entry = NULL;
    element_t *safe = NULL;
    bool is_dup = false;

    list_for_each_entry_safe(entry, safe, head, list) {
        if ((&safe->list != head) && !strcmp(entry->value, safe->value)) {
            list_del(&entry->list);
            q_release_element(entry);
            is_dup = true;
        } else if (is_dup) {
            list_del(&entry->list);
            q_release_element(entry);
            is_dup = false;
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (head == NULL || list_empty(head)) {
        return;
    }

    struct list_head *node, *safe;

    list_for_each_safe(node, safe, head) {
        list_move(node, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/

    if (head == NULL || list_empty(head) || k == 1) {
        return;
    }

    int groups_left = q_size(head) / k;
    int count = k;
    struct list_head *curr = head->next;
    struct list_head *safe;
    struct list_head *dummy = head;

    while (groups_left > 0) {
        while (count > 0) {
            safe = curr->next;
            list_move(curr, dummy);
            curr = safe;
            count--;
        }
        groups_left--;
        if (groups_left == 0) {
            break;
        }
        count = k - 1;
        dummy = curr->prev;
        curr = curr->next;
    }
}

/* Merge two sorted queues into one sorted queue, which is in ascending order */
void q_merge_two(struct list_head *h1, struct list_head *h2)
{
    // https://leetcode.com/problems/merge-two-sorted-lists/

    if (h1 == NULL || h2 == NULL) {
        return;
    }

    LIST_HEAD(result);
    struct list_head *curr1 = h1->next;
    struct list_head *curr2 = h2->next;
    struct list_head *safe1, *safe2;

    while (curr1 != h1 && curr2 != h2) {
        if (cmp(curr1, curr2) < 0) {
            safe1 = curr1->next;
            list_move_tail(curr1, &result);
            curr1 = safe1;
        } else {
            safe2 = curr2->next;
            list_move_tail(curr2, &result);
            curr2 = safe2;
        }
    }

    while (curr1 != h1) {
        safe1 = curr1->next;
        list_move_tail(curr1, &result);
        curr1 = safe1;
    }

    while (curr2 != h2) {
        safe2 = curr2->next;
        list_move_tail(curr2, &result);
        curr2 = safe2;
    }

    list_splice(&result, h1);
    INIT_LIST_HEAD(h2);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    // list_sort(head, cmp);
    // if (descend)
    //     q_reverse(head);

    if (head == NULL || list_empty(head) || list_is_singular(head)) {
        return;
    }

    struct list_head *slow = head->next;
    struct list_head *fast = head->next->next;

    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    LIST_HEAD(left);
    list_cut_position(&left, head, slow);

    q_sort(&left, 0);
    q_sort(head, 0);
    q_merge_two(head, &left);

    if (descend) {
        q_reverse(head);
    }
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/

    if (head == NULL || list_empty(head)) {
        return 0;
    }

    struct list_head *curr = head->next;
    struct list_head *safe;
    struct list_head *iter;

    while (curr != head) {
        safe = iter = curr->next;
        while (iter != head) {
            if (cmp(curr, iter) > 0) {
                list_del(curr);
                q_release_element(list_entry(curr, element_t, list));
                break;
            }
            iter = iter->next;
        }
        curr = safe;
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/

    if (head == NULL || list_empty(head)) {
        return 0;
    }

    struct list_head *curr = head->next;
    struct list_head *safe;
    struct list_head *iter;

    while (curr != head) {
        safe = iter = curr->next;
        while (iter != head) {
            if (cmp(curr, iter) < 0) {
                list_del(curr);
                q_release_element(list_entry(curr, element_t, list));
                break;
            }
            iter = iter->next;
        }
        curr = safe;
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/

    if (head == NULL) {
        return 0;
    }
    if (head->next == head) {
        return q_size(list_entry(head, queue_contex_t, chain)->q);
    }

    size_t listsize = 0;
    struct list_head *node;
    list_for_each(node, head) {
        listsize++;
    }

    struct list_head *front = head->next;
    struct list_head *tail = head->prev;
    while (listsize > 1) {
        size_t i = 0;
        size_t j = listsize - 1;
        while (i < j) {
            q_merge_two(list_entry(front, queue_contex_t, chain)->q,
                        list_entry(tail, queue_contex_t, chain)->q);
            front = front->next;
            i++;
            tail = tail->prev;
            j--;
        }
        listsize = (listsize + 1) / 2;
        front = head->next;
    }

    return q_size(list_entry(head->next, queue_contex_t, chain)->q);
}
