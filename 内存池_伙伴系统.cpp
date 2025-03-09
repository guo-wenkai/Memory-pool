#define _CRT_SECURE_NO_WARNINGS //��һ�������ڵ�һ��
#include <stdio.h> //��׼��������ļ�
#include <stdlib.h>
#include <assert.h>

#define m 16 //�ڴ��������2^16��65536��WORD_b

typedef struct WORD_b {   //���ϵͳ���� (���ṹ)
    struct WORD_b* llink;//ǰ��ָ��
    int tag;             //��ʶ,0����,1ռ��
    int kval;            //���С,2���ݴ�k(�������kֵ)
    struct WORD_b* rlink;//���ָ��
}WORD_b,*Space_b;
typedef struct HeadNode {//�����ÿռ��
    int nodesize;       //������Ŀ��п��С
    WORD_b* first;      //�����ͷָ��
}FreeList[m + 1];

static Space_b g_pav;//�ڴ�ص���ʼ��ַ

void InitMem(FreeList *pf)//��������ʼ���ڴ��
{
    //�����ڴ��
    g_pav = (WORD_b*)malloc((1<<m) * sizeof(WORD_b));//1<<1->2 ,1<<2->4,1<<3->8

    assert(g_pav != NULL);
    if (g_pav == NULL)
    {
        printf("�ڴ�س�ʼ��ʧ��!!!\n");
        return;
    }
    //��ʼ���ڴ��
    g_pav->llink = g_pav->rlink = g_pav;
    g_pav->tag = 0;
    g_pav->kval = m;


    //��ʼ�������ÿռ��
    for (int i = 0; i < m+1; i++)
    {
        (*pf)[i].nodesize = 1 << i;
        (*pf)[i].first = NULL;
    }
    (*pf)[m].first = g_pav;//���ڴ�ӵ������ÿռ��
}

//�����ڴ�
//pf�����ÿռ��,n�������(WORD_b)
WORD_b* MyMalloc(FreeList* pf, int n)
{
    if (n <= 0)
        return NULL;
    int i;
    for (i = 0; i < m + 1; i++)//�Һ��ʵĿ��п�
    {
        //���п��С>=n�Һ����н��
        if ((*pf)[i].nodesize >= n && (*pf)[i].first != NULL)
            break;
    }
    if (i == m + 1)//û���ҵ�
        return NULL;
    //����i����ĵ�һ�����(1.���������޳�,2.����ʣ�ಿ��)
    WORD_b* p = (*pf)[i].first;
    if (p->rlink == p)//Ψһ�Ľ��
        (*pf)[i].first = NULL;
    else
    {
        (*pf)[i].first = p->rlink;//ͷָ��ָ����һ�����
        //��p���������޳�
        p->llink->rlink = p->rlink;
        p->rlink->llink = p->llink;
    }

    //����ʣ�ಿ��(��������,�Ӵ�С)
    for (i=i-1; (*pf)[i].nodesize >= n; i--)
    {
        WORD_b* q = p + (*pf)[i].nodesize; //���ѵ��½ڵ�
        q->llink = q->rlink = q;
        q->tag = 0;
        q->kval = i;

        (*pf)[i].first = q;
    }

    //��������ȥ���ڴ�
    p->tag = 1;
    p->kval = i + 1;

    return p;
}

//�ͷ�ռ�ÿ�p�������ÿռ��pf��
void MyFree(FreeList* pf, WORD_b* p)
{
    if (p == NULL)
        return;
    WORD_b* q; //����ַ
    int flg = 0;//flg=0,p�����;flg=1,p���ҿ�
    int i;
    for (i = p->kval; i < m; i++)
    {
        //�ж��Լ���˭(����黹���ҿ�)?
        if ((p - g_pav) % (1 << (p->kval + 1)) == 0)//p-g_pav:��Ե�ַ,  p�����
        {
            flg = 0;//p�����
            q = p + (1 << p->kval);
        }
        else //p���ҿ�
        {
            flg = 1;//p���ҿ�
            q = p - (1 << p->kval);
        }

        if (q->tag == 1)//�����ռ�ÿ�
            break;
        else //����ǿ��п�,��Ҫ�ϲ�
        {
            //�ѻ��q����Ӧ��������ɾ��
            if (q->rlink == q)//��ǰ����ֻ��һ�����
                (*pf)[i].first = NULL;
            else //��ֹһ�����,��Ҫ��q���������޳�
            {
                if ((*pf)[i].first == q)//q��������ǰ��Ľ��
                    (*pf)[i].first = q->rlink;
                //��q�޳�
                q->llink->rlink = q->rlink;
                q->rlink->llink = q->llink;
            }
            if (flg == 0)//p�����,�ѻ��ϲ���p��
            {
                p->tag = 0;
                p->kval += 1; //ָ��+1
            }
            else //p���ҿ�,��p�ϲ������q��
            {
                q->kval += 1;
                p = q;
            }
        }
    }
    
    //��p���뵽��Ӧ��λ��
    if ((*pf)[i].first == NULL)//p�ǲ���ĵ�һ�����
    {
        p->llink = p->rlink = p;
        (*pf)[i].first = p;
    }
    else //p���ǵ�ǰ����ĵ�һ�����
    {
        WORD_b* p1 = (*pf)[i].first->llink;//���һ�����,Ҳ����Ϊp��ǰ��
        WORD_b* p2 = (*pf)[i].first;//��һ�����,Ҳ����Ϊp�ĺ��
        p->llink = p1;
        p->rlink = p2;
        p1->rlink = p;
        p2->llink = p;
    }
}

//���ռ�ÿ���Ϣ
void Show(WORD_b *p)
{
    if (p == NULL)
        return;
    printf("ռ�ÿ�,��ʶ=%d,��С=%d\n",p->tag,1<<p->kval);
}

//��������ÿռ������н����Ϣ
void ShowPav(FreeList* pf)
{
    WORD_b* p;
    for (int i = 0; i < m + 1; i++)
    {
        if ((*pf)[i].first == NULL)
            continue;
        p = (*pf)[i].first;
        printf("���п��С:%d\n",1<<p->kval);
        do
        {
            printf("    ���п�:���=%d,��С=%d\n",p->tag,1<<p->kval);
            p = p->rlink;
        } while (p != (*pf)[i].first);
    }
}


int main()
{
    /*for (int i = 0; i < 10; i++)
    {
        printf("2^%d = %d\n",i,1<<i);
    }*/

    FreeList fl;
    InitMem(&fl);

    ShowPav(&fl);

    printf("-----------------------\n");

    WORD_b* p1 = MyMalloc(&fl,500);
    WORD_b* p2 = MyMalloc(&fl, 500);
    WORD_b* p3 = MyMalloc(&fl, 500);
    WORD_b* p4 = MyMalloc(&fl, 500);
    Show(p1);
    Show(p2);
    Show(p3);
    Show(p4);
    ShowPav(&fl);

    printf("-----------------------\n");
    MyFree(&fl,p2); p2 = NULL; //p2��p3���ǻ��
    MyFree(&fl, p3); p3 = NULL;
    ShowPav(&fl);

	return 0;
}