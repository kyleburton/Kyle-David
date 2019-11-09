#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <unistd.h>

/**
 * Linked List examples
 *
 *
 */

typedef struct Node {
  void*        value;
  struct Node* next;
} Node;

static Node ll_end;

Node* ll_new() {
  return &ll_end;
}

Node* ll_alloc() {
  Node *n = (Node*)malloc(1 * sizeof(Node));
  if (!n) {
    fprintf(stderr, "%s(%d).%s: Error allocating memory: errno=%d; error=%s\n", __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno));
    return NULL;
  }

  return n;
}

Node* ll_add(Node* head, void* value) {
  Node *nhead = ll_alloc();
  if (!nhead) {
    return NULL;
  }

  nhead->value = value;
  nhead->next = head;

  return nhead;
}

void ll_pretty_print(char* name, Node* head, void (*pprint) (FILE*, void*)) {
  Node *cur = head;
  printf("List{%s}|", name);
  while (cur != &ll_end) {
    printf("(");
    pprint(stdout, cur->value);

    if (cur->next == &ll_end) {
      printf(",END)|");
      break;
    }

    printf(",*)->");
    cur = cur->next;
  }
  printf("\n");
}


void ll_print_pointer(FILE* fp, void* value) {
  fprintf(fp, "%p", value);
}

void ll_print(char *name, Node* head) {
  ll_pretty_print(name, head, ll_print_pointer);
}

void ll_vfree_noop(__attribute__((unused)) void* value) {
  return;
}

void ll_free(Node** head, void (vfree)(void*)) {
  Node* cur = *head;
  while (cur != &ll_end) {
    Node* next = cur->next;
    // david - try putting the free before the vfree and see what valgrind reports
    vfree(cur->value);
    free(cur);
    cur = next;
  }
  *head = NULL;
}

/******************************************************************************/

void my_printval(FILE* fp, void* value) {
  fprintf(fp, "\"%s\"", (char*)value);
}

void test_01() {
  Node *l1 = ll_new();
  ll_pretty_print("emtpy", l1, my_printval);
  l1 = ll_add(l1, "David");
  ll_pretty_print("has1 ", l1, my_printval);
  l1 = ll_add(l1, "Kyle");
  ll_pretty_print("has2 ", l1, my_printval);

  ll_free(&l1, ll_vfree_noop);
}

/*********************************************************************************/
char* ask_user_for_string(char *prompt) {
  size_t buff_size = 8192;
  char *buff = malloc(buff_size * sizeof(char));
  if (!buff) {
    fprintf(stderr, "%s(%d).%s: Error allocating memory: errno=%d; error=%s\n", __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno));
    return NULL;
  }

  bzero(buff, buff_size);
  printf("%s: ", prompt);
  fflush(stdout);
  /* size_t num_read = read(0, buff, buff_size-1); */
  /* if (num_read >= buff_size) { */
  /*   fprintf(stderr, "%s(%d).%s: Error: you typed too much for me :(\n", __FILE__, __LINE__, __FUNCTION__); */
  /*   return NULL; */
  /* } */

  /* buff[num_read] = '\0'; */
  /* fprintf(stderr, "READ(%zu)[%s]\n", num_read, buff); */

  if (!fgets(buff, buff_size, stdin)) {
    fprintf(stderr, "%s(%d).%s: Error reading from stdin: errno=%d; error=%s\n", __FILE__, __LINE__, __FUNCTION__, errno, strerror(errno));
    return NULL;
  }
  // NB: this is bad form
  *(strchr(buff,'\n')) = '\0';
  fprintf(stderr, "READ[%s]\n", buff);


  return buff;
}

void test_02 () {
  Node *l1 = ll_new();

  l1 = ll_add(l1, ask_user_for_string("How are you feeling?"));
  l1 = ll_add(l1, ask_user_for_string("How loud is the sun?"));
  l1 = ll_add(l1, ask_user_for_string("When is second breakfast?"));
  l1 = ll_add(l1, ask_user_for_string("How many Professor X's to an ocelot?"));

  printf("\n");
  ll_pretty_print("brought-to-you-by...", l1, my_printval);

  ll_free(&l1, free);
}

/*********************************************************************************/
int main (__attribute__((unused)) int argc, __attribute__((unused)) char** argv) {
  printf("Hello, Linked Lists\n");

  test_01();
  test_02();

  return 0;
}