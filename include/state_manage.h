#define STATE_BUF_SIZE 1024
#define STATE_TOKEN "#"
#define STATE_FILE "./state.dat"


struct car_state
{
	char type;
	int value;
}* state_array;

struct state_node
{
	struct state_node *before, *next;
	struct car_state state;
}

struct state_list
{
	int size;
	struct state_node* head;
	struct state_node* tail;
}g_state_list;

struct car_state** load_car_state_() // 배열 동적 할당. 
{
	File* fd;
	char* it;
	char buffer[STATE_BUF_SIZE];
	int i=0;

// read file
	fd = fopen(STATE_FILE,"r");

	if(fd == NULL)
	{
		printf("state file open error\n");
		return NULL;
	}
	
	if(fgets(buffer,STATE_INFO_SIZE, fd)==NULL)
		return NULL;

	fclose(fd);

// parsing buffer.
 
	// transfer size first. 
	it = strtok(buffer,STATE_TOKEN);
	if(it == NULL)
		return NULL;

	// alloction array. 
	state_array = (struct car_state*)malloc(sizeof(struct car_state)*it);

	while(it!=NULL)
	{
		int tmp;

		it = strtok(buffer,STATE_TOKEN);
		if(it != NULL)
		{
			tmp = atoi(it);
			state_array[i].type= tmp/10000;
			state_array[i++].value = tmp%10000;
		}
	}
}
	// state_list에 존재하는 node들을 순회하면서 파일에 저장한다. 
void save_car_state()
{
	File* fd = NULL;
	char* buffer = (char*)malloc(sizeof(char)*STATE_BUF_SIZE);
	struct state_node* it = g_state_list.head;
	int i;
	char* tmp;

	sprintf(tmp,"%d",g_state_list.size);
	strcat(buffer,tmp);

	// 리스트 변환해서 버퍼에 넣는다. 
	for(i=0; i<g_state_list.size;i++)
	{
		sprintf(tmp,"%d",it->state.type);
		strcat(buffer,tmp);
		sprintf(tmp,"%d",it->state.value);
		strcat(buffer,tmp);
		strcat(buffer,STATE_TOKEN);
	}
	// 파일을 열어서 버퍼의 값들을 파일에 써준다.  	
	strcat(buffer,NULL);

	fd = fopen(STATE_FILE,"w+t");
	if(fd == NULL)
		printf("");

	fwrite(buffer,sizeof(char),strlen(buffer),fd);
	free(buffer);
	fclose(fd);
}

void add_states(struct img_data* idata)
{
	
}
