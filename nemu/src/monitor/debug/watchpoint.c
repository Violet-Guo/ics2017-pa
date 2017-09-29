#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

WP* new_wp();
void free_wp(WP *wp);

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].exp[0] = '\0';
    wp_pool[i].value = -1;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp() {
  if (free_ == NULL) {
    assert(0);
  }

  WP *wp = free_;
  free_ = free_->next;
  wp->next = NULL;

  return wp;
}

void free_wp(WP *wp) {
  wp->exp[0] = '\0';
  wp->value = -1;
  wp->next = free_;
  free_ = wp;
}

void insert_wp(char *args) {
  bool flag = true;
  uint32_t val = expr(args, &flag);

  if (!flag) {
    printf("You input an invalid expression, failed to create watchpoint!");
    return ;
  }  

  WP *wp = new_wp();
  wp->value = val;
  strcpy(wp->exp, args);

  if (head == NULL) {
    wp->NO = 1;
    head = wp;
  }
  else {
    WP *wwp;
    wwp = head;
    while (wwp->next != NULL) {
      wwp = wwp->next;
    }
    wp->NO = wwp->NO + 1;
    wwp->next = wp;
  }

  return ;
}

void delete_wp(int no) {

  if (head == NULL) {
    printf("There is no watchpoint to delete!");
    return ;
  }

  WP *wp;
  if (head->NO == no) {
    wp = head;
    head = head->next;
    free_wp(wp);
  }
  else {
    wp = head;
    while (wp->next != NULL && wp->next->NO != no) {
      wp = wp->next;
    }
    if (wp == NULL) {
      printf("Failed to find the NO.%d watchpoint!", no);
    }
    else {
      WP *del_wp;
      del_wp = wp->next;
      wp->next = del_wp->next;
      free_wp(del_wp);
      printf("NO.%d  watchpoint has been deleted!\n", no);
    }
  }

  return ;
}

void display_wp() {
  if (head == NULL) {
    printf("There is no watchpoint!\n");
    return ;
  }

  WP *wp;
  printf("NO      expression        value\n");
  wp = head;
  while (wp != NULL) {
    printf("%-5d   %-15s   %-16u\n", wp->NO, wp->exp, wp->value);
    wp = wp->next;
  }
}

int * haschanged() {
  WP *wp = head;
  bool flag = true;
  uint32_t val;
  static int no[NR_WP];
  int i = 0;

  while (wp != NULL) {
    val = expr(wp->exp, &flag);
    if (val != wp->value) {
      wp->value = val;
      no[i++] = wp->NO;
    }
      wp = wp->next;
  }
  no[i] = -1;

  return no;
}
