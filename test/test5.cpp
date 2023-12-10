#include<cstdio>
#include<iostream>
#include<vector>
#include<cmath>
#include<fstream>
#include<stack>
#include<algorithm>

using namespace std;

class Point {
public:
    int x, y;

    double angle;

    Point() {}

    Point(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    void calculateAngle(Point* p) {
        angle = atan2(this->y, this->x) - atan2(p->y, p->x);
    }
};

int n;
vector<Point*>ar;
string filename = "1305015_input.txt";
stack<Point*>S;


double euclideanDistance(Point* a, Point* b) {
    int dx = a->x - b->x;
    int dy = a->y - b->y;
    return sqrt(dx * dx + dy * dy);
}

bool func(Point* a, Point* b)
{
    if(a->angle != b->angle)
        return a->angle < b->angle;
    return euclideanDistance(ar[0], a) > euclideanDistance(ar[0], b);
}

void takeInput()
{
    ifstream myfile;
    myfile.open(filename);
    if(myfile)
    {
        myfile >> n;

        int now = 0;
        int x, y;

        while(now < n)
        {
            if(myfile >> x >> y)
            {
                Point* p = new Point(x, y);
                ar.push_back(p);
            }
            else
            {
                cout << "some problem" << endl;
                return;
            }
            now++;
        }
    }
    else
    {
        cout << "could not open file" << endl;
        return;
    }
}

void findRightmostLowestPoint()
{
    int id = 0; // rightmost lowest point index

    for(int i = 1; i < n; ++i)
        //if((ar[i]->x > ar[id]->x)  ||  (ar[i]->x == ar[id]->x  &&  ar[i]->y < ar[id]->y))
        if((ar[i]->y < ar[id]->y)  ||  (ar[i]->y == ar[id]->y  &&  ar[i]->x < ar[id]->x))
            id = i;

    swap(ar[0], ar[id]);
}

void sortAllPoints()
{
    for(int i = 0; i < n; ++i)
    {
        ar[i]->calculateAngle(ar[0]);
    }
    sort(ar.begin() + 1, ar.end(), func);
}

Point* secondElement(stack<Point*>&S)
{
    Point* top = S.top();
    S.pop();
    Point* second = S.top();
    S.push(top);
    return second;
}

int isLeft(Point* a, Point* b, Point* c)
{
    int halfArea =  a->x * (b->y - c->y) +
                    b->x * (c->y - a->y) +
                    c->x * (a->y - b->y);
    if(halfArea > 0)
        return 1;
        //nicher gula thek just false return kore dilei hoy
    else if(halfArea > 0)
        return -1;
    else
        return 0;
}

void printStack(stack<Point*>&S)
{
    cout << S.size() << endl;
    stack<Point*>temp;
    while(S.empty() == false)
    {
        Point* p = S.top();
        S.pop();
        temp.push(p);
        cout << p->x << " " << p->y << endl;
    }
    //restoring the stack in actual condition.
    while(temp.empty() == false)
    {
        Point* p = temp.top();
        temp.pop();
        S.push(p);
    }
}

void grahamScan()
{
    //Point* p = new Point();
    //S.push(p);
    S.push(ar[0]);
    S.push(ar[1]);
    //S.push(ar[2]);

    int i = 2;
    //int i = 3;
    while(i < n)
    {
        //cout << i << " : " ;
        //printStack(S);
        if(isLeft(secondElement(S), S.top(), ar[i]) == 1)
        {
            S.push(ar[i]);
            i++;
        }
        else
        {
            //cout << "popped" << endl;
            S.pop();
        }
    }
}

int main()
{
    takeInput();
    findRightmostLowestPoint();
    sortAllPoints();

    grahamScan();

    printStack(S);

    return 0;
}