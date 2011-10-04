#ifndef  __WORK_FUNC_H__
#define  __WORK_FUNC_H__


extern volatile unsigned char 		flag_pre_work;
extern volatile unsigned char 		flag_work;

extern volatile unsigned short 		dac_h1;
extern volatile unsigned short 		dac_h2;

extern volatile unsigned short 		cal_U;
extern volatile unsigned short 		cal_I;

extern volatile unsigned short 		num_rate;
extern volatile unsigned short 		num_group;

extern volatile unsigned short 		del_t;
extern volatile unsigned short 		u_anode;

extern const char  					*text_group[3];
extern const char  					*text_rate[4];

extern unsigned short				index_group;
extern unsigned short				index_mode;

extern volatile unsigned short 		new_del_t;
extern volatile unsigned short 		new_u_anode;
extern volatile unsigned short 		test_del_t;
extern volatile unsigned short 		test_u_anode;

extern unsigned char				start_flag;
extern unsigned char				fire_flag;
extern unsigned char				wait_flag;
extern unsigned char				warm_flag;
extern unsigned char				warm_flag_down;
extern unsigned char				flag_pree_any_key;
extern unsigned char				flag_ready_fire;
extern unsigned char				flag_ext_menu;
extern unsigned char				flag_password_access;

extern char							*passwd;

extern int 							count_cmd;

extern char 						str[69];
extern char 						str1[69];
extern char 						str2[69];
extern char 						rec_char;
extern short						v_array[250];



void update_menu(void);
void update_menu_for_main(void);
void update_menu_edit(void);
void update_menu_test(void);
void update_passwd(void);
void update_log(void);

void dac1(void); 				//Подготовка рабочего режима
void dac2(void); 				//Работа рентгена
void warming_up_on_start(void); //предварительнй прогрев
void warming_up(void);
void warming_down(void); 		//Выключение предварительного прогрева
void check_button(void);
void update_adc(void);
void handler_adc(void);

void set(void);
void set1(void);
void set2(void);
void set3(void);
void set4(void);
void set5(void);
void set6(void);

void fire(void);

void set_1(void);
void set_group(void);
void set_dac_h1(void);
void set_num_group(void);
void set_new_u_anode(void);
void set_test_u_anode(void);

void set_2(void);
void set_mode(void);
void set_dac_h2(void);
void set_num_rate(void);
void set_new_t_exposure(void);
void set_test_t_exposure(void);

void goto_menu1(void);
void goto_menu2(void);

void menu(void);
void menu_next_1(void);
void menu_next_2(void);

void set_pos();
void set_char();

void press_down_anykey(void);
void chech_uart(void);

void set_inc();
void set_dec();

void set_cal_u_anode_inc(void);
void set_cal_u_anode_dec(void);

void set_cal_k_i_inc(void);
void set_cal_k_i_dec(void);

void set_cal_k_u_inc(void);
void set_cal_k_u_dec(void);

void set_cal_m(void);
void set_cal_s(void);
void set_cal_i(void);
void set_cal_u(void);

void update_menu_cal_m1(void);
void update_menu_cal_m2(void);

void update_menu_cal_i(void);
void update_menu_cal_u(void);

#endif
