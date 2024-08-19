#include "sdb.h"

static WP wp_pool[NR_WP] = {};
static WP *free_ = NULL;
WP *head = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }
  head = NULL;
  free_ = wp_pool;
}

WP* new_wp()
{
  WP *op=head;
  if(head == NULL)
  {
    head = free_;
    free_ = free_->next;
    head->next = NULL;
    return head;
  }
  else
  {
    if(free_ == NULL)
      assert(0);
    else
    {
      while(op->next!=NULL)
        op = op->next;
      op->next = free_;
      free_ = free_->next;
      op = op->next;
      op->next = NULL;
    }
  }
  return op;
}

void watch_info()
{
  WP *op=head;
  printf("\033[41mNum\tType\tEnb\tExpr\033[0m\n");
  while (op!=NULL)
  {
    printf("%d\twp\t%d\t%s\n", op->NO, op->enb, op->expr);
    op=op->next;
  }
}

bool free_wp_no(int w_no)
{
  WP *op = head;
  while(op!=NULL)
  {
    if(op->NO == w_no)
    {
      free_wp(op);
      return true;
    }
    if(op->next==NULL)
      break;
    op = op->next;
  }
  return false;
}

void free_wp(WP *wp)
{
  WP *op = head;
  WP *last_wp = NULL;
  while (op!=NULL)
  {
    if(head == wp)
    {
      wp->enb = 0;
      memset(wp->expr, '\0', sizeof(wp->expr));
      head = head->next;
      wp->next = free_;
      free_ = wp;
      break;
    }
    else if (op == wp)
    {
      wp->enb = 0;
      memset(wp->expr, '\0', sizeof(wp->expr));
      last_wp->next = op->next;
      wp->next = free_;
      free_ = wp;
    }
    last_wp = op;
    op = op->next;
  }
  
}