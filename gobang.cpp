#include <cstdio> 
#include <cstdlib> 
#include <algorithm>

#define N 9
#define WIN 5
#define DEP 3
#define INF 0x3f3f3f3f

#define WHITE 0
#define EMPTY 1
#define BLACK 2
#define ENEMY(a) (2-a)

using std::sort;

int n,l[6*N-2];
int *p[6*N-2][N];
int m[N][N];
void init() 
{
	for(int i=0;i<N;i++)
	{
		for(int j=0;j<N;j++)
			p[n][j]=&m[i][j];
		l[n++]=N;
		for(int j=0;j<N;j++)
			p[n][j]=&m[j][i];
		l[n++]=N;
	} 
	for(int i=0;i<N;i++)
	{
		int t=N-i;
		if(t<WIN) continue;
		for(int j=0;j<t;j++)
			p[n][j]=&m[j][i+j];
		l[n++]=t;
	}
	for(int i=1;i<N;i++)
	{
		int t=N-i;
		if(t<WIN) continue;
		for(int j=0;j<t;j++)
		{
			p[n][j]=&m[i+j][j];
		}
		l[n++]=t;
		for(int j=0;j<t;j++)
		{
				p[n][j]=&m[N-j-1][i+j];
				
		}
		
		l[n++]=t;
	}
	for(int i=0;i<N;i++)
	{
		int t=i+1;
		if(t<WIN) continue;
		for(int j=0;j<t;j++)
			p[n][j]=&m[i-j][j];
		l[n++]=t;
	}
}
int table[WIN]={0,1,5,25,125};
int _score(int color,bool &win)
{
	win=false;
	int ans=0;
	for(int i=0;i<n;i++)
	{
		int run=0;
		bool last_empty=false;
		for(int j=0;j<l[i];j++)
		{
			if(*p[i][j]!=color)
			{
				if(run>=WIN)
				{
					win=true;
					return INF;
				}
				//结算run
				if(!last_empty&&(*p[i][j]!=EMPTY)) run=0;//两边都堵上没有贡献
				if(!last_empty||(*p[i][j]!=EMPTY)) run--;//堵一边贡献-1
				ans+=table[run];
				run=0;
				last_empty=(*p[i][j]==EMPTY);
			}
			else
				run++;
		}
		if(run>=WIN)
		{
			win=true;
			return INF;
		}
		//结算run（这次一定右边被堵上了） 
		if(run&&last_empty) ans+=table[run-1];
	}
	return ans;
}
int winner()
{
	for(int i=0;i<n;i++)
	{
		int run=0;
		int color=-1;
		for(int j=0;j<l[i];j++)
		{
			if(*p[i][j]!=color)
			{
				if(run>=WIN)
					return color;
				run=1;
				color=*p[i][j];
			}
			else if(*p[i][j]==color&&*p[i][j]!=EMPTY)
				run++;
		}
		if(run>=WIN&&color!=EMPTY)
		{
			return color;
		}
	}
	return EMPTY;
}
void print();
int score(int color)
{
	bool win,lose;
	int mine,his;
	mine=_score(color,win);
	his=_score(ENEMY(color),lose);
	if(win&&lose)
	{
		print();
		puts("ERROR");
		fflush(stdout);
		abort();
	}
	if(win) return INF;
	if(lose) return -INF;
	return mine-his;
}
inline int abs(int a)
{
    return a<0?-a:a;
}
inline int min(int a,int b)
{
    return a<b?a:b;
} 
int px[N*N],py[N*N],pieces;
struct point
{
    int x,y,w;
    void calc_w()
    {
        w=INF;
        for(int i=0;i<pieces;i++)
            w=min(w,abs(px[i]-x)+abs(py[i]-y));
    }
    bool operator<(point b) const
    {
        return w<b.w;
    }
}buf[DEP+1][N*N];
inline void add(int x,int y)
{
    px[pieces]=x;
    py[pieces]=y;
    pieces++;
}
int alphabeta(int color,int dep,int a=-INF,int b=INF)
{
	if(!dep) return score(color);
	
	int win=winner();
	if(win==color) return INF;
	else if(win==ENEMY(color)) return -INF;

    //试图使用sort来使得alpha-beta剪枝更有效
    /*
       结果：原先(gobang-3.cpp)：_score占59.66%的时间，in.txt花87.35秒运行    排序部分共计约34.52%
       现在：_score占85.38%的时间，in.txt花30秒时间运行，排序部分共计约10%
    */
	int top=0;
	for(int i=0;i<N;i++)
        for(int j=0;j<N;j++)
        {
            buf[dep][top].x=i;
            buf[dep][top].y=j;
            buf[dep][top].calc_w();
            top++;
        }
    sort(buf[dep],buf[dep]+top);
    for(int i=0;i<top;i++)
    {
        int x=buf[dep][i].x,y=buf[dep][i].y;
		if(m[x][y]==EMPTY)
		{
			m[x][y]=color;
            add(x,y);
			int v=-alphabeta(ENEMY(color),dep-1,-b,-a);
            pieces--;
			m[x][y]=EMPTY;
			if(v>=b) return b;
			if(v>a) a=v;
		}
    }
	return a;
}	
void AI(int color)
{
	int x=-1,y=-1;
	int mn=INF;
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++)
            if(m[i][j]==EMPTY)
            {
				m[i][j]=color;
                add(i,j);
				int v=alphabeta(ENEMY(color),DEP);
                pieces--;
				m[i][j]=EMPTY;
				
				if(v<=mn)
				{
					mn=v;
					x=i;
					y=j;
				}
			}
    add(x,y);
	m[x][y]=color;
}
char out[3]={'O','.','X'};
void print()
{
	putchar('\n');
	putchar(' ');
	for(int i=0;i<N;i++)
		printf(" %d",i);
	putchar('\n');
	for(int i=0;i<N;i++)
	{
		printf("%c ",'A'+i);
		for(int j=0;j<N;j++)
			printf("%c ",out[m[i][j]]);
		putchar('\n');
	}
}
int main()
{
	puts("Welcome to the gobang game! Five-in-A-row to win");
    puts("You move first,you will use 'X'.");
	puts("Enter your move in the form line(letter)+col(number)");
	puts("e.g A3 C0");
	init();
	for(int i=0;i<N;i++) for(int j=0;j<N;j++) m[i][j]=EMPTY;
	print();
	int left=N*N;
	while(left)
	{
		int x,y;
		while(true)
		{
            //col & row
			char c;
			int r;
			scanf(" %c%d",&c,&r);
			x=c-'A';
			y=r;
			if(x>=0&&x<N&&y>=0&&y<N)
				if(m[x][y]!=EMPTY)
				{
					puts("You can't put on an occupied square!'");
				}
				else
				{
					break;
				}
			else
			{
				puts("Out of bounds!");
			}
		}
        add(x,y);
		m[x][y]=BLACK;
		if(winner()==BLACK)
		{
			puts("You win!!");
			goto end;
		}
		if(left==1) break;
		puts("AI thinking...");
		AI(WHITE);
		if(winner()==WHITE)
		{
			puts("You lose :(");
			goto end;
		}
		print();
		left-=2;
	}
	puts("You tied with the computer.");
	end:;print();
	return 0;
}
