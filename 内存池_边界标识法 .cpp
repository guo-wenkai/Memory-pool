#define _CRT_SECURE_NO_WARNINGS //��һ�������ڵ�һ��
#include <stdio.h> //��׼��������ļ�
#include <stdlib.h>

//����ͷ�ڵ���״���Ϸ�

typedef struct WORD {//��
    union {
        WORD* llink; //ͷ����,ָ��ǰ�����
        WORD* uplink;//�ײ���,ָ�򱾽��ͷ��
    };
    int tag;//��ʶ,0����,1ռ��,ͷ����β������
    int size;//ͷ����,���С,��WORDΪ��λ
    WORD* rlink;//ͷ����,ָ���̽��
}WORD, * Space;//Space:�����ÿռ�ָ������

#define SIZE 10000  //�ڴ�صĴ�С(WORD)
#define e 10 //��Ƭ�ٽ�ֵ,��ʣ��Ŀ��п�С��eʱ,�������п鶼�����ȥ

static Space FootLoc(Space p)//ͨ��p����p��β
{
    return p + p->size - 1;
}

Space InitMem()//��ʼ���ڴ��
{
    //Space pav = (Space)malloc(SIZE * sizeof(WORD));//�ڴ��
    //���ڴ�ص�ǰ��ཨ"һ��ǽ",��ֹ���պϲ�ʱԽ��
    Space pav = (Space)malloc((SIZE+2) * sizeof(WORD));//�ڴ��+2��ǽ
    pav->tag = 1;//��ǽ
    pav++;

    //����pav��ͷ
    pav->llink = pav;
    pav->rlink = pav;
    pav->tag = 0;
    pav->size = SIZE;

    //����p��β
    Space p = FootLoc(pav);//pָ��β
    p->uplink = pav;
    p->tag = 0;

    (p + 1)->tag = 1;//��ǽ

    return pav;
}

//���ڴ��pav,����n��WORD,�ɹ����������ڴ�ĵ�ַ,ʧ�ܷ���NULL
//�����״���Ϸ�
WORD* MyMalloc(Space* pav, int n)
{
    if (*pav == NULL)//�ڴ���Ѿ�����
        return NULL;
    
    Space p = *pav;
    do //�״���Ϸ�,�ҵ�һ�����������Ŀ��п�
    {
        if (p->size >= n)//�ҵ���
            break;
        p = p->rlink;
    } while (p != *pav);

    if (p->size < n)//û�����������Ŀ��п�
        return NULL;

    if (p->size - n < e)//�������п鶼����
    {
        WORD* q = p;
        //q->llink,q->rlink,ռ�ÿ鲻��Ҫ����
        q->tag = 1;//ռ��
        //q->size ���ı�
        p = FootLoc(p);
        //p->uplink���ı�
        p->tag = 1;//ռ��

        //��q���������޳�
        if (q->rlink == q)//����������ֻ��q��һ�����
            *pav = NULL;
        else//�ж�����
        {
            *pav = q->rlink;

            q->llink->rlink = q->rlink;
            q->rlink->llink = q->llink;
        }

        return q;
    }
    else //���п�һ���ַ����ȥ,������(�ߵ�ַ)�ֳ�ȥ
    {
        *pav = p->rlink;
        p->size -= n; //p�ָ����´�С
        WORD *p2 = FootLoc(p);//�½���β
        p2->uplink = p;
        p2->tag = 0;

        //����ռ�ÿ�
        WORD* q = p2+1;//ָ����Ҫ���صĵ�ַ
        q->tag = 1;//ռ��
        q->size = n;//����Ĵ�С
        p2 = FootLoc(q);//q��β��
        p2->uplink = q;
        p2->tag = 1;

        return q;
    }
}

void MyFree(Space* pav, WORD* p)//�ͷ�p
{
    WORD* pl = p - 1;//����β��
    WORD* pr = FootLoc(p) + 1;//�ҿ��ͷ
    if (pl->tag == 1 && pr->tag == 1)//���ռ��,�ҿ�ռ��,ֱ�Ӳ���
    {
        p->tag = 0; //�ͷź��ǿ��п�
        FootLoc(p)->tag = 0;
        if (*pav == NULL)//�����ÿռ��ΪNULL,p�ǵ�һ�����
        {
            *pav = p;
            p->llink = p->rlink = p;
        }
        else
        { //��p������pav��ǰ��,��p��Ϊ�����ÿռ������һ�����
            WORD* p1 = *pav;//��һ�����
            WORD* p2 = p1->llink;//���һ�����

            //��p������p1��p2���м�,��p1��ǰ��,p2�ĺ���
            p->rlink = p1;
            p1->llink = p;
            p->llink = p2;
            p2->rlink = p;
        }
    }
    else if (pl->tag == 0 && pr->tag == 1)//���Ϊ���п�,�ҿ�Ϊռ�ÿ�
    {//ֻ��Ҫ��p�ӵ��������漴��
        WORD* q = pl->uplink;//����ͷ
        q->size += p->size; //�ϲ�����п�Ĵ�С

        //�����¿��β
        FootLoc(q)->tag = 0;
        FootLoc(q)->uplink = q;
    }
    else if (pl->tag==1 && pr->tag==0)//���Ϊռ�ÿ�,�ҿ�Ϊ���п�
    {//���ҿ�ӿ����ÿռ���޳�,�ٰ��ҿ�ϲ���p������,�ٰ�p���뵽�����ÿռ����
        //1.p���ҿ�pr�ӿ����ÿռ���޳�
        if (pr->rlink == pr)//ֻ��һ�����н��
        {
            *pav = NULL;
        }
        pr->llink->rlink = pr->rlink;
        pr->rlink->llink = pr->llink;

        //2.���ҿ�ϲ���p������
            //����pͷ����Ϣ
        p->tag = 0;  
        p->size += pr->size;
            //����p��β����Ϣ
        FootLoc(p)->tag = 0;
        FootLoc(p)->uplink = p;

        //3.��p���뵽�����ÿռ��(������pav��ǰ��)
        if (*pav == NULL)//p�ǵ�һ�����
        {
            p->llink = p->rlink = p;
            *pav = p;
        }
        else//�����ÿռ�����������,��p������pav��ǰ��,��p��Ϊ�����ÿռ������һ�����
        {
            WORD* p1 = *pav;//��һ�����
            WORD* p2 = p1->llink;//���һ�����

            //��p������p1��p2���м�,��p1��ǰ��,p2�ĺ���
            p->rlink = p1;
            p1->llink = p;
            p->llink = p2;
            p2->rlink = p;
        }
    }
    else //���Ϊ���п�,�ҿ�ҲΪ���п�
    {//���ҿ���������޳�,�ٰ���,p,������ϲ�
        //1.���ҿ���������޳�
        pr->llink->rlink = pr->rlink;
        pr->rlink->llink = pr->llink;

        //2.����,p,�Һϲ�
           //�����¿��ͷ
        pl = pl->uplink;//�ҵ�pl(���)��ͷ
        pl->size = pl->size + p->size + pr->size;
           //�����¿��β��
        FootLoc(pl)->uplink = pl;
        FootLoc(pl)->tag = 0;
    }
}

void Show(WORD* p)//���Ժ���,���p������
{
    printf("p�ĵ�ַ:%p,p->tag:%d,p->size:%d(WORD),p��β�͵�tag:%d\n",
        p,p->tag,p->size,FootLoc(p)->tag);
}

void ShowMem(Space pav)//����ڴ�ص����п��п�
{
    if (pav == NULL)
    {
        printf("�ڴ���ѿ�\n");
        return;
    }
    Space p = pav;
    do
    {
        printf("���п�,��ַ:%p,tag:%d,size:%d,β��tag:%d\n",
            p,p->tag,p->size,FootLoc(p)->tag);
        p = p->rlink;
    } while (p != pav);
}

int main()
{
    Space pav = InitMem();//��������ʼ���õ��ڴ��
    WORD* p1 = MyMalloc(&pav, 1000);
    WORD* p2 = MyMalloc(&pav, 1500);
    WORD* p3 = MyMalloc(&pav, 500);
    WORD* p4 = MyMalloc(&pav, 1000);
    WORD* p5 = MyMalloc(&pav, 1000);

    Show(p1);
    Show(p2);
    Show(p3);
    Show(p4);
    Show(p5);
    ShowMem(pav);

    printf("-----------\n");
    /*MyFree(&pav, p2); p2 = NULL;
    MyFree(&pav, p1); p1 = NULL;
    MyFree(&pav, p3); p3 = NULL;*/
    //�������Ҷ��ǿ��п�
    MyFree(&pav, p2); p2 = NULL;
    MyFree(&pav, p4); p4 = NULL;
    MyFree(&pav, p3); p3 = NULL;

    MyFree(&pav, p1); p1 = NULL;
    MyFree(&pav, p5); p5 = NULL;
    ShowMem(pav);

	return 0;
}