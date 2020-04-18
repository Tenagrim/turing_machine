#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#define UP_ARROW (char)'A'
#define LEFT_ARROW (char)'D'
#define RIGHT_ARROW (char)'C'
#define DOWN_ARROW (char)'B'

const char	EMPTY_S = '_';//λ
const char	ONE_S = '1';
const char	ZERO_S = '0';
const char	SPACE_S = ' ';
const int	SCR_W = 60;
const int	SCR_H = 40;
const int	START_LINE = 4;
//const char	ST_FILE[] = "s_t2.txt";
const char	ST_FILE[] = "states.txt";

typedef struct	{
	int	q;
	char	a;
	char	mv;
}		state;

void		gotoxy(int x,int y)
{
    printf("%c[%d;%df",0x1B,y,x);
}

void		printxy(int x, int y, char c)
{
    printf("%c[%d;%df%c",0x1B,y,x,c);
}

void		ft_putchar(char c)
{
	write(1, &c ,1);
}

int		ft_strlen(char *str)
{
	return ((*str) ? ft_strlen(++str) + 1 : 0);
}

char		getch()
{
	char c;
	system("stty raw");
	c = getchar();
	system("stty sane");
	gotoxy(1,1);
	printf("      ");
	return(c);
}

int		kbhit(void)
{
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if(ch != EOF)
	{
		ungetc(ch, stdin);
		return 1;
	}

	return 0;
}

void		clrscr()
{
	system("clear");
	return;
}

int checkKeysPressed(int direction)
{
	int pressed;
	
	if(kbhit())
	{
		pressed=getch();
		if (direction != pressed)
		{
			if(pressed == DOWN_ARROW)
				direction = pressed;
			else if (pressed == UP_ARROW)
				direction = pressed;
			else if (pressed == LEFT_ARROW)
				direction = pressed;
			else if (pressed == RIGHT_ARROW)
				direction = pressed;
		}
	}
	return(direction);
}

void		init_strip(char **arr, int size )
{
	int i;

	i = 0;
	arr[0] = malloc(size);
	while(i < size)
	{
		arr[0][i] = EMPTY_S;
		i++;
	}
}

int		is_in_alph(char *alph, char c)
{
	while (*alph)
	{
		if (*alph == c)
			return (1);
		alph++;
	}
	return 0;
}

void		init_scr()
{
	int i;
	int j;

	i = 1;
	while(i <= SCR_H)
	{
		j = 1;
		while(j <= SCR_W)
		{
			gotoxy(i,j);
			printf(" ");
			j++;
		}
		i++;
	}
}

void		show_strip(char *s, int size, int pos)
{
	int i;
	int s_i;
	int f_i;
	int end_s;

	s_i = pos - SCR_W / 2;	
	f_i = pos + SCR_W / 2;
	end_s = 1;
	if (s_i < 0)
	{
		end_s = -s_i;
		end_s++;
		s_i = 0;
	}
	//clrscr();
	i = 0;
	gotoxy(1,START_LINE);
	while(i < end_s -1)
	{
		printf(" ");
		i++;
	}
	i = s_i;
	gotoxy(end_s,START_LINE);
	while(i < f_i)
	{
		//printf("!!!!!!!!!\n");
		printf("%c", s[i]);
		i++;
	}
	gotoxy(SCR_W / 2 + 1,1 + START_LINE);
	printf("^");
	gotoxy(1,1);
	fflush(stdout);
}

void 		control(char *strip, int size, int *pos, char *alph)
{
	char	c;
	int	i;

	c = '0';
	i = 0;
	while(c !='q')
	{

		c = getch();
		//printf("|%c|\n  >", c);
		//printf("%d ", i);
		//ft_putchar(c);
		//if (c == UP_ARROW)
			//printf("UP\n");
		//else if (c == DOWN_ARROW)
			//printf("DOWN\n");
		if (c == LEFT_ARROW)
			 //printf("LEFT\n");
			*pos = *pos - 1;
		else if (c == RIGHT_ARROW)
			//printf("RIGHT\n");
			*pos = *pos + 1;
		else if (is_in_alph(alph, c))
			strip[*pos] = c;
		else if (c == ' ')
			strip[*pos] = '_'; 
		//else 
			//printf("XZ\n");
		i++;
		//printf("(%c)",UP_ARROW);
		show_strip(strip, size, *pos);

	}
}

void		init_states(state ****arr, int size_h, int size_w)
{
	int	i;
	int	j;
	state	***states;

	i = 0;
	states = (state***)malloc(sizeof(state**) * size_h);
	while (i < size_h)
	{
		states[i] = (state**)malloc(sizeof(state*) * size_w);
		j = 0;
		while (j < size_w)
		{
			states[i][j] = (state*)malloc(sizeof(state));
			j++;
		}
		i++;
	}
	*arr = states;
}

void		load_states(char *filename, state ****arr, int *size, char **alph)
{
	FILE	*f;
	char	*str;
	int	tr;
	int	i;
	int	j;
	int	size_w;
	state	***states;
	
	str = malloc(256);
	*alph = malloc(256);
	//str = NULL;
	f = fopen(filename, "r");

	fscanf(f, "%d", size);
	fscanf(f, "%s", *alph);

	//gotoxy(10,50);
	//printf("%s" , *alph);
	//gotoxy(1,1);

	size_w = ft_strlen(*alph);

	init_states(&states, *size, size_w);
	//printf("++ %d ++\n", *size);
	i = 0;
	while (i < *size)
	{
		//fscanf(f, "%d\t%d %c %c\t%d %c %c\t%d %c %c\n", &tr, &states[i][0]->q, &states[i][0]->a, &states[i][0]->mv, &states[i][1]->q, &states[i][1]->a, &states[i][1]->mv, &states[i][2]->q, &states[i][2]->a, &states[i][2]->mv );
		j = 0;
		fscanf(f, "%d", &tr);
		while (j < size_w)
		{
			fscanf(f, "%d %c %c\t", &states[i][j]->q, &states[i][j]->a, &states[i][j]->mv);
			j++;
		}
		
		i++;
	}
	//gotoxy(10,10);
	//printf("|%d| |%c| |%c|\n", states[0][1]->q, states[0][1]->a, states[0][1]->mv);
	//gotoxy(1,1);
	*arr = states;
	free(str);
	//*size = 1;
}

void		print_states(state ***states, int count, char *alph)
{
	int i;
	int j;
	int width;

	i = 0;
	width = ft_strlen(alph);
	gotoxy(1, 7);
	printf("Алфавит: %s", alph);
	gotoxy(1, 8);
	printf("Таблица переходов:");
	gotoxy(1, 9);
	printf("  \t|\t");
	while (i < width)
	{
		printf("\"%c\"  \t    |\t", alph[i]);
		i++;
	}
	i = 0;
	while(i < count)
	{	
		j = 0;
		gotoxy(1, i + 10);
		printf("q%d:\t|\t",i + 1);
		while(j < width)
		{
			printf("q%d\t%c %c |\t", states[i][j]->q, states[i][j]->a, states[i][j]->mv);
			j++;
		}
		printf("\n");
		i++;
	}
	gotoxy(1,1);
}

void		mark_state(int cur, int ind, char *m)
{
	int x_pos;


	//x_pos = 55;
	gotoxy(ind * 16 + 14, cur + 10);
	printf("%s",m);
	gotoxy(1,1);
}

void		delay(int ms)
{
	int c;
	int i;

	i = 0;
	c = clock() + ms;
	while(clock() < c)
		i++;
}

void		getch_async(char *c)
{
	while (1)
	{
		*c = getch();
	}
}

int		get_w_ind(char *alph, char c)
{
	int pos;

	pos = 0;
	while(*alph)
	{
		if(*alph == c)
			return (pos);
		alph++;
		pos++;
	}
	return (-1);
}

void		process(state ***states, char *strip, int size, int *pos, char *alph)
{
	state		*c_state;
	int		c_ind;
	char		c;
	//int		delta;
	int		x_pos;
	//char		stop;

	c_ind = 0;
	x_pos = 0;
	c = '0';
	//delta = clock();
	//mark_state(0, "<==");


		//gotoxy(70,35);


		//x_pos = get_w_ind(alph, strip[*pos]);
		//printf("%d", x_pos);
		//fflush(stdout);
		//sleep(10);
	do{

		//gotoxy(70,10);
		//printf("%d  %d", c_ind, x_pos);
		mark_state(c_ind,x_pos , "  ");
		x_pos = get_w_ind(alph, strip[*pos]);
		/*if (strip[*pos] == '_')
		{
			c_state = states[c_ind][0];
			x_pos = 0; 
		}
		else if (strip[*pos] == '0')
		{
			c_state = states[c_ind][1];
			x_pos = 1; 
		}
		else if (strip[*pos] == '1')
		{
			c_state = states[c_ind][2];
			x_pos = 2; 
		}*/
		c_state = states[c_ind][x_pos];
		strip[*pos] = c_state->a;
		c_ind = c_state->q - 1;
		mark_state(c_ind, x_pos,  "=>");
		if (c_state->mv == '<')
			*pos = *pos - 1; 
		else if (c_state->mv == '>')
			*pos = *pos + 1; 
		show_strip(strip, size, *pos);
		//c = getch(); 
		delay(70000);
		gotoxy(1,1);
	}while (c_ind != -2 && c != 'q');
}

void		wait_()
{
	char c;

	c = '0';
	while (c != 'q')
		c = getch();
}

void		cleanup(state ***states, int size, char *strip,  char *alph)
{
	int	i;
	int	j;
	int	width;

	i = 0;
	width = strlen(alph);
	while (i < size)
	{
		j = 0;
		while(j < width)
		{
			free(states[i][j]);
			j++;
		}
		free(states[i]);
		i++;
	}
	free(states);
	free(alph);
	free(strip);
}

int		main(int argc, char **argv)
{
	char	*strip;
	int	size;
	int	pos;
	state	***states;
	int	s_count;
	char	*alph;

	size = 300;
	pos = 150;
	init_scr();
	init_strip(&strip, size);
	show_strip(strip,size, pos);
	//load_states(argv[1], &states, &s_count, &alph);
	//print_states(states, s_count, alph);
	//gotoxy(60, 60);
	//printf("%d", ft_strlen("12345"));
	//gotoxy(1, 1);

	while (1)
	{
		load_states(argv[1], &states, &s_count, &alph);
		print_states(states, s_count, alph);
		control(strip, size, &pos, alph);
		process(states, strip,size,  &pos, alph);
		cleanup(states, s_count, strip, alph);
		//ait_();
	}
	cleanup(states, s_count, strip, alph);
}
