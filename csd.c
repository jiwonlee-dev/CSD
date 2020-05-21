#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pbc.h>

#define LEVEL 10
#define TESTPARAM 30

typedef struct param{
	element_t g;
	element_t g1;
	element_t g2;
	element_t g3;
	element_t h0[LEVEL];
	element_t h1[LEVEL];
	element_t k0[LEVEL];
	element_t k1[LEVEL];
}param;

typedef struct SK{
	element_t a0;
	element_t a1;
	element_t b[LEVEL];
	element_t k0[LEVEL];
	element_t k1[LEVEL];
}SK;

typedef struct SKID{
	int id;
	SK *sk[LEVEL];
}SKID;

typedef struct HDR{
	element_t A0;
	element_t A1;
}HDR;

typedef struct CT
{
	HDR hdr;
	element_t C;
}CT;

typedef struct SET
{
	char c[LEVEL];
	char d[LEVEL];
}SET;

int isBit(int element, int index)
{
	int x = 1;
	x <<= index;
	x &= element;

	return (x!=0);
}

char* getBit(int i) {
	static char bits[LEVEL+1] = {'0','0','0','0','0','0','0','0','0','0'};
	int bits_index = 10;
	int j = 0x01;
	while ( i > 0 ) {
		bits[bits_index--] = (i & j ) + '0';
		i = ( i >> 1);
	}
	return bits;
}

param* init_param(pairing_t pairing){

	int i;
	param *p;
	p = (param*)malloc(sizeof(param));
	element_init_G1(p->g, pairing);
	element_init_G1(p->g1, pairing);
	element_init_G1(p->g2, pairing);
	element_init_G1(p->g3, pairing);
	for(i=0;i<LEVEL;i++){
		element_init_G1(p->h0[i], pairing);
		element_init_G1(p->h1[i], pairing);
		element_init_G1(p->k0[i], pairing);
		element_init_G1(p->k1[i], pairing);
	}
	return p;
}

SK* init_SK(pairing_t pairing){
	int i;
	SK* sk;
	sk = (SK*)malloc(sizeof(SK));
	element_init_G1(sk->a0, pairing);
	element_init_G1(sk->a1, pairing);
	for(i=0; i<LEVEL; i++){
		element_init_G1(sk->b[i], pairing);
		element_init_G1(sk->k0[i], pairing);
		element_init_G1(sk->k1[i], pairing);
	}
	return sk;
}

SKID* init_SKID(int id, pairing_t pairing){
	int i;
	SKID* skID;
	skID = (SKID*)malloc(sizeof(SKID));
	skID->id = id;
	for(i = 0; i< LEVEL; i++){
		skID->sk[i] = init_SK(pairing);
	}
	return skID;
}

void setup(param *p, pairing_t pairing, element_t* master_key){

	clock_t start, end;
	int i;

	element_t alpha;

	//start = clock();

	element_init_Zr(alpha, pairing);

	element_random(p->g);
	element_random(p->g2);
	element_random(p->g3);

	for(i=0;i<LEVEL;i++){
		element_random(p->h0[i]);
		element_random(p->h1[i]);
		element_random(p->k0[i]);
		element_random(p->k1[i]);
	}
	element_random(alpha);
	element_pow_zn(p->g1, p->g, alpha);
	element_pow_zn(*master_key, p->g2, alpha);
	//end = clock();
	element_clear(alpha);
}

SKID* keyGen(element_t master_key, param *p, pairing_t pairing, int id){
	SKID* skID;
	int i, j;

	element_t temp;
	element_t r;

	element_init_G1(temp, pairing);
	element_init_Zr(r, pairing);

	skID = init_SKID(id, pairing);
	for(i=0; i<LEVEL; i++){
		skID->sk[i] = init_SK(pairing);
		element_set(temp, p->g3);
		element_random(r);
		for(j=0; j< LEVEL; j++){
			if(isBit(id, j)){
				element_mul(temp, temp, p->h1[j]);
				element_pow_zn(skID->sk[i]->b[j], p->h0[j], r);
			}
			else{
				element_mul(temp, temp, p->h0[j]);
				element_pow_zn(skID->sk[i]->b[j], p->h1[j], r);
			}
		}
		//printf("\n");
		if(isBit(id, i)){
			element_mul(temp, temp, p->k0[i]);
		}
		else{
			element_mul(temp, temp, p->k1[i]);
		}
		//element_printf("h: %B\n", temp);
		element_pow_zn(temp, temp, r);
		//element_printf("mk : %B\n", master_key);
		//element_printf("temp : %B\n", temp);
		element_mul(skID->sk[i]->a0, master_key, temp);

		element_pow_zn(skID->sk[i]->a1, p->g, r);

		for(j=0;j<LEVEL;j++){
			if(j == i){
				if(isBit(id, i)){
					//skID->sk[i]->k0[j] = NULL;
					element_pow_zn(skID->sk[i]->k1[j], p->k1[j], r);
				}
				else{
					//skID->sk[i]->k1[j] = NULL;
					element_pow_zn(skID->sk[i]->k0[j], p->k0[j], r);
				}
			}
			else{
				element_pow_zn(skID->sk[i]->k0[j], p->k0[j], r);
				element_pow_zn(skID->sk[i]->k1[j], p->k1[j], r);
			}
		}
	}
	skID->id = id;
	element_clear(temp);
	element_clear(r);
	return skID;
}

CT encrypt(param *p, pairing_t pairing, SET S, char* msg){
	CT ct;
	int i;
	char buf[1024]={0};
	element_t temp1, temp2;
	element_t s;
	element_t key;

	element_init_GT(key, pairing);
	element_init_GT(temp1, pairing);
	element_init_GT(ct.C, pairing);
	element_init_G1(ct.hdr.A0, pairing);
	element_init_G1(ct.hdr.A1, pairing);
	element_init_G1(temp2, pairing);
	element_init_Zr(s, pairing);

	element_random(s);
	pairing_apply(temp1, p->g1, p->g2, pairing);
	element_pow_zn(key, temp1, s);

	element_pow_zn(ct.hdr.A0, p->g, s);

	element_set(temp2, p->g3);

	//printf("c :\t");
	for(i=0;i<LEVEL;i++){
		//printf("%d\t", S.c[i]);	
	}

	//printf("d :\t");
	for(i=0;i<LEVEL;i++){
		//printf("%d\t", S.d[i]);	
	}
	printf("\n");
	for(i = 0;i<LEVEL; i++){
		if(S.c[i] == 0){
			//printf("c0 : %d\n", i);
			element_mul(temp2, temp2, p->h0[i]);
		}
		else if(S.c[i] == 1){
			//printf("c1 : %d\n", i);
			element_mul(temp2, temp2, p->h1[i]);
		}
		else if(S.c[i] == 2){
			//printf("c2 : %d\n", i);
			element_mul(temp2, temp2, p->h0[i]);
			element_mul(temp2, temp2, p->h1[i]);
		}

		if(S.d[i] == 0){
			//printf("d0 : %d\n", i);
			element_mul(temp2, temp2, p->k0[i]);
		}
		else if(S.d[i] == 1){
			//printf("d1 : %d\n", i);
			element_mul(temp2, temp2, p->k1[i]);
		}
	}
	element_pow_zn(ct.hdr.A1, temp2, s);


	element_from_bytes(temp1, msg);
	//element_printf("K : %B\n", temp1);
	element_mul(ct.C, key, temp1);

	element_clear(temp1);
	element_clear(temp2);
	element_clear(s);
	return ct;
}

char* decrypt(SKID *skID, SET S, CT ct, pairing_t pairing){
	char *msg;
	int len;
	int i, j;
	element_t temp1, temp2;
	element_t B;
	element_t key;

	for(j=0; j < LEVEL; j++){
		//printf("sk : %d %d\n", isBit(skID->id, j), S.d[j]);
		if((isBit(skID->id, j) && S.d[j] == 0) || (!isBit(skID->id, j) && S.d[j] == 1)){
			break;
		}
	}
	if(j == LEVEL){
		printf("error dec\n");
		return NULL;
	}

	//printf("dec j : %d\n", j);
	element_init_GT(temp1, pairing);
	element_init_GT(temp2, pairing);
	element_init_GT(key, pairing);
	element_init_G1(B, pairing);

	element_set(B, skID->sk[j]->a0);
	//printf("c :\t");
	for(i=0; i<LEVEL; i++){
		if(S.c[i] == 2){
			//printf("%d\t", i);
			element_mul(B, B, skID->sk[j]->b[i]);
		}
	}
	//printf("j %d\n", j);
	//printf("\nd :\t");
	for(i=0; i<LEVEL; i++){
		//printf("%d\t", S.d[i]);
		if(!(S.d[i] == 2) && !(i == j)) {
			if(S.d[i] == 1){
				//printf("%d\t", i);
				element_mul(B, B, skID->sk[j]->k1[i]);
			}
			else{
				//printf("%d\t", i);
				element_mul(B, B, skID->sk[j]->k0[i]);
			}
		}
	}
	//printf("\n");

	pairing_apply(temp1, ct.hdr.A0, B, pairing);
	pairing_apply(temp2, skID->sk[j]->a1, ct.hdr.A1, pairing);
	element_div(key, temp1, temp2);
	element_div(temp1, ct.C, key);
	len = element_length_in_bytes(temp1);
	msg = (char*)malloc(sizeof(char)*(len+1));
	msg[len] = 0;
	element_to_bytes(msg, temp1);
	element_clear(temp1);
	element_clear(temp2);
	element_clear(key);
	element_clear(B);
	return msg;
}
int main()
{
	pairing_t pairing;
	char params[1024];
	size_t count;
	int i;

	element_t master_key;
	element_t sig;
	SK *sk, *mk, *sk2;
	SK *testSK[TESTPARAM];
	CT ct;
	char *c;
	param *p;
	char msg[] = "hello";
	char msg2[] = "hello2";
	SET S;
	time_t start;
	FILE *fp;

	fp = fopen("element", "w");

	printf("csd start\n");
	count = fread(params, 1, 1024, stdin);
	if (!count) pbc_die("input error");
	pairing_init_set_buf(pairing, params, count);

	p = init_param(pairing);
	element_init_G1(master_key, pairing);
	printf("setup\n");
	setup(p, pairing, &master_key);
	element_out_str(fp, 32, master_key);
	printf("keygen\n");
	sk = keyGen(master_key, p, pairing, 2);
	//sk2 = keyGen(master_key, p, pairing, 4);

	/*
	for(i = 0 ; i < TESTPARAM; i++){
		testSK[i] = keyGen(master_key, p, pairing, i);
	}
	*/

	// ***** - 0**0*
	for(i=0;i<LEVEL;i++){
		S.c[i] = 2;
		S.d[i] = 2;
	}
	S.d[1] = 0;

	printf("Enc Start\n");
	start = clock();
	ct = encrypt(p, pairing, S, msg);
	printf("Enc End : %f\n",(float)(clock() - start)/CLOCKS_PER_SEC);
	/*
	for(i = 0;i<TESTPARAM;i++){
		c = decrypt(testSK[i], S, ct, pairing);
		//element_printf("C : %B\n", ct.C);
		if(c != NULL)
			printf("msg %s: %s\n", getBit(i), c);
	}
	*/



	printf("Dec Start\n");
	start = clock();
	c = decrypt(sk, S, ct, pairing);
	printf("Dec End : %f\n",(float)(clock() - start)/CLOCKS_PER_SEC);
	//element_printf("C : %B\n", ct.C);
	if(c != NULL)
		printf("msg %s: %s\n", getBit(i), c);
	return 0;

}

