#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pbc.h>

#define LEVEL 20
#define K 10
#define USER 1 << LEVEL
#define TESTPARAM 30

typedef struct param{
	element_t g;
	element_t g1;
	element_t g2;
	element_t g3L;
	element_t g3R;
	element_t h1L[LEVEL];
	element_t h1R[LEVEL];
}param;

typedef struct SK{
	int bits;
	char* id;
	element_t a0;
	element_t a1;
	element_t b[LEVEL];
}SK;

typedef struct DL{
	SK *skWL;
	SK *skLS[LEVEL];
}DL;

typedef struct DR{
	SK *skWR;
	SK *skRS[LEVEL];
}DR;

typedef struct D{
	int id;
	DL *dl;
	DR *dr;
}D;

typedef struct HDR
{
	element_t h0;
	element_t h1;
}HDR;

typedef struct CT{
	element_t ct;
	HDR Hl;
	HDR Hr;
}CT;

typedef struct SET
{
	int s;
	int e;
}SET;

char* right_sibling(int w, int j){
	char *result= (char*)malloc(sizeof(char)*(j+2));
	if(!isBit(w, j)){
		for(int i = 0; i<j;i++){
			result[i] = isBit(w, i) +'0';
		}
		result[j] = '1';
		result[j+1] = 0;
		return result;
	}
	else return NULL;
}

char* left_sibling(int w, int j){
	char *result= (char*)malloc(sizeof(char)*(j+2));
	if(isBit(w, j)){
		for(int i = 0; i<j;i++){
			result[i] = isBit(w, i) +'0';
		}
		result[j] = '0';
		result[j+1] = 0;
		return result;
	}
	else return NULL;
}

int isBit(int element, int index)
{
	int x = 1;
	element >>= LEVEL-index-1;
	x &= element;

	return (x!=0);
}

char* getBit(int i) {
	char* bits;//{'0','0','0','0','0','0','0','0','0','0'};
	int bits_index = LEVEL-1;
	int j = 0x01;
	bits = (char*)malloc(sizeof(char)*(LEVEL+1));
	for(j = 0; j<LEVEL;j++){
		bits[j] = '0';
	}
	bits[LEVEL] = 0;
	j = 0x01;
	while ( i > 0 ) {
		bits[bits_index--] = (i & j ) + '0';
		i = ( i >> 1);
	}
	return bits;
}

void FL(element_t* result, char* v, int bits, param p, pairing_t pairing){
	element_set1(*result);

	//printf("fl v bits : %s %d\n", v, bits);
	for(int i = 0;i<bits+1; i++){
		//printf("%c", v[i]);
		if(v[i] == '1'){
			element_mul(*result, *result, p.h1L[i]);
		}
	}
	//printf("\n");
	element_mul(*result, *result, p.g3L);
}

void FR(element_t *result, char* v, int bits, param p, pairing_t pairing){
	element_set1(*result);
	//printf("fr v bits : %s %d\n", v, bits);
	for(int i = 0;i<bits+1; i++){
		//printf("%c", v[i]);
		if(v[i] == '1'){
			element_mul(*result, *result, p.h1R[i]);
		}
	}
	//printf("\n");
	element_mul(*result, *result, p.g3R);
}

param* init_param(pairing_t pairing){

	int i;
	param *p;
	p = (param*)malloc(sizeof(param));
	element_init_G1(p->g, pairing);
	element_init_G1(p->g1, pairing);
	element_init_G1(p->g2, pairing);
	element_init_G1(p->g3L, pairing);
	element_init_G1(p->g3R, pairing);
	for(i=0;i<LEVEL;i++){
		element_init_G1(p->h1L[i], pairing);
		element_init_G1(p->h1R[i], pairing);
	}
	return p;
}

SK* init_SK(int bits, pairing_t pairing){
	int i;
	SK* sk;
	sk = (SK*)malloc(sizeof(SK));
	sk->bits = bits;
	element_init_G1(sk->a0, pairing);
	element_init_G1(sk->a1, pairing);
	for(i=0; i<LEVEL; i++){
		element_init_G1(sk->b[i], pairing);
	}
	return sk;
}

DL* genDL(int id, element_t secret, param p, pairing_t pairing){
	int i;
	DL* dl;
	dl = (DL*)malloc(sizeof(DL));

	element_t r;
	element_init_Zr(r, pairing);
	element_random(r);
	//element_set1(r);

	element_t temp, fl;
	element_init_G1(temp, pairing);
	element_init_G1(fl, pairing);

	FL(&fl, getBit(id), LEVEL-1, p, pairing);
	element_pow_zn(temp, fl, r);
	dl->skWL = init_SK(LEVEL-1, pairing);
	element_mul(dl->skWL->a0, secret, temp);
	element_pow_zn(dl->skWL->a1, p.g, r);

	/*
	   int cnt;
	   for(i = 0, cnt = 0 ; i<LEVEL; i++){
	   if(left_sibling(id, i) != -1) cnt++;
	   }
	   dl->skLS = (element_t*)malloc(sizeof(element_t)*cnt);
	 */
	for(i = 0; i< LEVEL; i++){
		char* leftsb = left_sibling(id, i);
		if(leftsb != NULL){
			//printf(" %d left sb : %s\n", i, leftsb);
			dl->skLS[i] = init_SK(i, pairing);
			dl->skLS[i]->id = leftsb;
			FL(&fl, leftsb, i, p, pairing);
			element_random(r);
			//element_set1(r);
			element_pow_zn(temp, fl, r);
			element_mul(dl->skLS[i]->a0, secret, temp);

			element_pow_zn(dl->skLS[i]->a1, p.g, r);
			for(int j = i+1; j< LEVEL; j++){
				element_pow_zn(dl->skLS[i]->b[j], p.h1L[j], r);
			}

		}
		else dl->skLS[i] = NULL;
	}
	element_clear(r);
	element_clear(temp);
	element_clear(fl);
	return dl;
}

DR* genDR(int id, element_t secret, param p, pairing_t pairing){
	int i;
	DR* dr;
	dr = (DR*)malloc(sizeof(DR));

	element_t r;
	element_init_Zr(r, pairing);
	element_random(r);
	//element_set1(r);

	element_t temp, fr;
	element_init_G1(temp, pairing);
	element_init_G1(fr, pairing);

	FR(&fr, getBit(id), LEVEL-1, p, pairing);
	element_pow_zn(temp, fr, r);
	dr->skWR = init_SK(LEVEL-1, pairing);
	element_mul(dr->skWR->a0, secret, temp);
	element_pow_zn(dr->skWR->a1, p.g, r);

	/*
	   int cnt;
	   for(i = 0, cnt = 0 ; i<LEVEL; i++){
	   if(right_sibling(id, i) != -1) cnt++;
	   }
	   dr->skWR = (element_t*)malloc(sizeof(element_t)*cnt);
	 */
	for(i = 0; i< LEVEL; i++){
		char* rightsb = right_sibling(id, i);
		if(rightsb != NULL){
			//printf("%d right sb : %s\n", i,rightsb);
			dr->skRS[i] = init_SK(i, pairing);
			dr->skRS[i]->id = rightsb;
			FR(&fr, rightsb, i, p, pairing);
			element_random(r);
			//element_set1(r);
			element_pow_zn(temp, fr, r);
			element_mul(dr->skRS[i]->a0, secret, temp);

			element_pow_zn(dr->skRS[i]->a1, p.g, r);
			for(int j = i+1; j< LEVEL; j++){
				element_pow_zn(dr->skRS[i]->b[j], p.h1R[j], r);
			}

		}
		else dr->skRS[i] = NULL;
	}
	element_clear(r);
	element_clear(temp);
	element_clear(fr);
	return dr;
}

void setup(param *p, pairing_t pairing, element_t* master_key){

	clock_t start, end;
	int i;

	element_t alpha;

	//start = clock();

	element_init_Zr(alpha, pairing);

	element_random(p->g);
	element_random(p->g2);
	element_random(p->g3L);
	element_random(p->g3R);

	for(i=0;i<LEVEL;i++){
		element_random(p->h1L[i]);
		element_random(p->h1R[i]);
	}
	element_random(alpha);
	element_pow_zn(p->g1, p->g, alpha);
	element_pow_zn(*master_key, p->g2, alpha);
	//end = clock();
	element_clear(alpha);
}

D* keyGen(element_t master_key, param p, pairing_t pairing, int id){
	D* d;
	DL* dl;
	DR* dr;
	int i, j;

	d = (D*)malloc(sizeof(D));

	element_t secret, secret2;
	element_t aw;

	element_init_G1(secret, pairing);
	element_init_G1(secret2, pairing);
	element_init_Zr(aw, pairing);

	element_random(aw);
	//element_set0(aw);

	element_pow_zn(secret, p.g2, aw);
	element_div(secret2, master_key, secret);
	dl = genDL(id, secret2, p, pairing);
	dr = genDR(id, secret, p, pairing);
	d->id = id;
	d->dl = dl;
	d->dr = dr;

	element_clear(secret);
	element_clear(secret2);
	element_clear(aw);
	return d;
}

CT encrypt(param p, pairing_t pairing, SET S, unsigned char* msg){
	CT ct;
	int i;
	char buf[1024]={0};
	element_t temp1, temp2;
	element_t s;
	element_t fl, fr;
	element_t key;

	element_init_GT(key, pairing);
	element_init_GT(temp1, pairing);
	element_init_GT(ct.ct, pairing);
	element_init_G1(ct.Hl.h0, pairing);
	element_init_G1(ct.Hl.h1, pairing);
	element_init_G1(ct.Hr.h0, pairing);
	element_init_G1(ct.Hr.h1, pairing);
	element_init_G1(temp2, pairing);
	element_init_G1(fl, pairing);
	element_init_G1(fr, pairing);
	element_init_Zr(s, pairing);

	element_random(s);
	//element_set1(s);
	pairing_apply(temp1, p.g1, p.g2, pairing);
	element_pow_zn(key, temp1, s);

	element_pow_zn(ct.Hl.h0, p.g, s);
	element_set(ct.Hr.h0, ct.Hl.h0);

	FL(&fl, getBit(S.s), LEVEL-1, p, pairing);
	element_pow_zn(ct.Hl.h1, fl, s);

	FR(&fr, getBit(S.e), LEVEL-1, p, pairing);
	element_pow_zn(ct.Hr.h1, fr, s);

	element_from_bytes(temp1, msg);
	//element_printf("K : %B\n", temp1);
	element_mul(ct.ct, key, temp1);

	element_clear(temp1);
	element_clear(temp2);
	element_clear(s);
	element_clear(key);
	return ct;
}

SK* skLDer(SK* sk, char* left, param p, pairing_t pairing){
	element_t t;
	element_t temp, temp2, temp3;
	SK* skL;

	element_init_G1(temp, pairing);
	element_init_Zr(t, pairing);
	skL = init_SK(sk->bits, pairing);
	skL->bits = sk->bits;
	skL->id = sk->id;
	element_set(skL->a0, sk->a0);
	element_set(skL->a1, sk->a1);
	for(int i = skL->bits+1; i<LEVEL;i++){
		element_set(skL->b[i], sk->b[i]);
	}
	for(int i = sk->bits+1; i<LEVEL; i++){
		element_random(t);
		//element_set1(t);

		//char* dumm = (char*)malloc(sizeof(char)*(i+1));
		//strncpy(dumm, left, i+1);
		//printf("dumm %s\n", dumm);

		FL(&temp, left, i, p, pairing);
		element_pow_zn(temp, temp, t);
		if(left[i] == '1') element_mul(temp, temp, sk->b[i]);
		element_mul(skL->a0, temp, skL->a0);

		element_pow_zn(temp, p.g, t);
		element_mul(skL->a1, temp, skL->a1);
		element_set0(skL->b[i]);
		for(int j=i+1; j<LEVEL; j++){
			element_pow_zn(temp, p.h1L[j], t);
			element_mul(skL->b[j], temp, skL->b[j]);
		}
	}
	element_clear(temp);
	element_clear(t);
	return skL;
}

SK* skRDer(SK* sk, char* right, param p, pairing_t pairing){
	element_t t;
	element_t temp;
	SK* skR;

	element_init_G1(temp, pairing);
	element_init_Zr(t, pairing);
	skR = init_SK(sk->bits, pairing);
	skR->bits = sk->bits;
	skR->id = sk->id;
	element_set(skR->a0, sk->a0);
	element_set(skR->a1, sk->a1);
	for(int i = skR->bits+1; i<LEVEL;i++){
		element_set(skR->b[i], sk->b[i]);
	}
	for(int i = sk->bits+1; i<LEVEL; i++){
		element_random(t);
		//element_set1(t);

		//char* dumm = (char*)malloc(sizeof(char)*(i+1));
		//strncpy(dumm, right, i+1);
		//printf("dumm %s\n", dumm);

		FR(&temp, right, i, p, pairing);
		element_pow_zn(temp, temp, t);
		if(right[i] == '1') element_mul(temp, temp, sk->b[i]);
		element_mul(skR->a0, temp, skR->a0);

		element_pow_zn(temp, p.g, t);
		element_mul(skR->a1, temp, skR->a1);
		element_set0(skR->b[i]);
		for(int j=i+1; j<LEVEL; j++){
			element_pow_zn(temp, p.h1L[j], t);
			element_mul(skR->b[j], temp, skR->b[j]);
		}
	}
	element_clear(temp);
	element_clear(t);
	return skR;
}

unsigned char* decrypt(D d, param p, SET S, CT ct, pairing_t pairing){
	unsigned char *msg;
	int len;
	int i, j;
	element_t temp1, temp2, temp3, temp4;
	element_t B;
	element_t t;
	element_t key;

	if(d.id < S.s || d.id > S.e){
		fprintf(stderr, "decrypt err\n");
		return NULL;
	}

	SK* skL;
	char* left = getBit(S.s);
	if(S.s != d.id){
		printf("left %s\n", left);
		for(i = 0 ;i< LEVEL;i++){
			if(d.dl->skLS[i] != NULL){
				if(!strncmp(left, d.dl->skLS[i]->id, d.dl->skLS[i]->bits+1))
				{
					printf("skLS %d : %s\n", i, d.dl->skLS[i]->id);
					break;
				}
			}
		}

		skL = skLDer(d.dl->skLS[i], left, p, pairing);
		//element_printf("skL a1 : %B\n", skL->a1);
		//printf("skL id : %s\n", skL->id);
	}
	else{
		skL = d.dl->skWL;
	}

	SK* skR;
	if(S.e != d.id){
		char* right = getBit(S.e);
		printf("right %s\n", right);

		for(i = 0 ;i< LEVEL;i++){
			if(d.dr->skRS[i] != NULL){
				if(strncmp(right, d.dr->skRS[i]->id, d.dr->skRS[i]->bits+1) == 0){
					printf("skRS %d : %s\n", i, d.dr->skRS[i]->id);
					break;
				}
			}
		}

		skR = skRDer(d.dr->skRS[i], right, p, pairing);
	}
	else{
		skR = d.dr->skWR;
	}

	element_init_GT(temp1, pairing);
	element_init_GT(temp2, pairing);
	element_init_GT(temp3, pairing);
	element_init_GT(temp4, pairing);

	//element_printf("skR a0 : %B\n", skR->a0);
	pairing_apply(temp1, ct.Hr.h0, skR->a0, pairing);
	//element_printf("up : %B\n", temp1);
	pairing_apply(temp2, skR->a1, ct.Hr.h1, pairing);
	//element_printf("down : %B\n", temp2);
	element_div(temp3, temp1, temp2);

	pairing_apply(temp1, ct.Hl.h0, skL->a0, pairing);
	//element_printf("up : %B\n", temp1);
	pairing_apply(temp2, skL->a1, ct.Hl.h1, pairing);
	//element_printf("down : %B\n", temp1);
	element_div(temp4, temp1, temp2);

	element_init_GT(key, pairing);
	element_mul(key, temp3, temp4);
	element_div(temp1, ct.ct, key);
	len = element_length_in_bytes(temp1);
	msg = (unsigned char*)malloc(sizeof(char)*(len+1));
	msg[len] = 0;
	element_to_bytes(msg, temp1);
	element_clear(temp1);
	element_clear(temp2);
	element_clear(temp3);
	element_clear(temp4);
	element_clear(key);
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
	//SKID *sk, *mk, *sk2;
	//SKID *testSK[TESTPARAM];
	CT ct;
	unsigned char *c;
	param *p;
	unsigned char msg[] = "hello";
	unsigned char msg2[] = "hello2";
	SET S;
	time_t start;

	D* d;

	//printf("%d' %dth left sibling : %s\n",12, 3,right_sibling(12, 10));




	count = fread(params, 1, 1024, stdin);
	if (!count) pbc_die("input error");
	pairing_init_set_buf(pairing, params, count);

	p = init_param(pairing);
	element_init_G1(master_key, pairing);
	setup(p, pairing, &master_key);
	d = keyGen(master_key, *p, pairing, 9);

	S.s = 1;
	S.e = (1<<LEVEL)-1;


	start = clock();
	ct = encrypt(*p, pairing,S, msg);
	printf("%f\n", (float)(clock() - start)/CLOCKS_PER_SEC);
	//element_printf("C : %B\n", ct.ct);

	start = clock();
	c = decrypt(*d, *p, S, ct, pairing);
	printf("%f\n", (float)(clock() - start)/CLOCKS_PER_SEC);
	if(c != NULL) printf("dec : %s\n", c);
	return 0;

}
