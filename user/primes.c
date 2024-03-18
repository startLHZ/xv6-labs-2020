#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

const uint INT_LEN = sizeof(int);

/**
 * @brief ��ȡ���ھӵĵ�һ������
 * @param lpipe ���ھӵĹܵ���
 * @param pfirst ���ڴ洢��һ�����ݵĵ�ַ
 * @return ���û�����ݷ���-1,�����ݷ���0
 */
int lpipe_first_data(int lpipe[2], int *dst)
{
  if (read(lpipe[RD], dst, sizeof(int)) == sizeof(int)) {
    printf("prime %d\n", *dst);
    return 0;
  }
  return -1;
}

/**
 * @brief ��ȡ���ھӵ����ݣ������ܱ�first������д�����ھ�
 * @param lpipe ���ھӵĹܵ���
 * @param rpipe ���ھӵĹܵ���
 * @param first ���ھӵĵ�һ������
 */
void transmit_data(int lpipe[2], int rpipe[2], int first)
{
  int data;
  // ����ܵ���ȡ����
  while (read(lpipe[RD], &data, sizeof(int)) == sizeof(int)) {
    // ���޷����������ݴ������ҹܵ�
    if (data % first)
      write(rpipe[WR], &data, sizeof(int));
  }
  close(lpipe[RD]);
  close(rpipe[WR]);
}

/**
 * @brief Ѱ������
 * @param lpipe ���ھӹܵ�
 */
void primes(int lpipe[2])
{
  close(lpipe[WR]);
  int first;
  if (lpipe_first_data(lpipe, &first) == 0) {
    int p[2];
    pipe(p); // ��ǰ�Ĺܵ�
    transmit_data(lpipe, p, first);

    if (fork() == 0) {
      primes(p);    // �ݹ��˼�룬���⽫��һ���µĽ����е���
    } else {
      close(p[RD]);
      wait(0);
    }
  }
  exit(0);
}

int main(int argc, char const *argv[])
{
  int p[2];
  pipe(p);

  for (int i = 2; i <= 35; ++i) //д���ʼ����
    write(p[WR], &i, INT_LEN);

  if (fork() == 0) {
    primes(p);
  } else {
    close(p[WR]);
    close(p[RD]);
    wait(0);
  }

  exit(0);
}
