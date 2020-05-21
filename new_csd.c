#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pbc.h>

#define STAR 2
#define LEVEL 10
#define TESTPARAM 30

typedef struct param{
	element_t g;
	element_t h0[LEVEL];
	element_t h1[LEVEL];
	element_t k0[LEVEL];
	element_t k1[LEVEL];
	element_t h_zero;
	element_t k_zero;
	element_t omega;
}param;

typedef struct SKID{
	int id;
	element_t x0;
	element_t x[LEVEL];
	element_t y0;
	element_t y[LEVEL * 2];
	element_t z;
}SKID;

typedef struct CT{
	element_t C0;
	element_t C1;
	element_t C2;
	element_t C3;
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

int getBit(int i) {
	static char bits[LEVEL+1] = {0};

	int bits_index = 10;
	int j = 0x01;
	while ( i > 0 ) {
		bits[bits_index--] = (i & j );
		i = ( i >> 1);
	}
	return (int)bits[0];
}

param* init_param(pairing_t pairing){

	int i;
	param *p;
	p = (param*)malloc(sizeof(param));
	element_init_G1(p->g, pairing);
	for(i=0;i<LEVEL;i++){
		element_init_G1(p->h0[i], pairing);
		element_init_G1(p->h1[i], pairing);
		element_init_G1(p->k0[i], pairing);
		element_init_G1(p->k1[i], pairing);
	}
	element_init_G1(p->h_zero, pairing);
	element_init_G1(p->k_zero, pairing);
	element_init_GT(p->omega, pairing);
	return p;
}

/*
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
*/

SKID* init_SKID(int id, pairing_t pairing){
	int i;
	SKID* skID;
	skID = (SKID*)malloc(sizeof(SKID));
	skID->id = id;
	element_init_G1(skID->x0, pairing);
	for(i = 0; i < LEVEL; i++){
		element_init_G1(skID->x[i], pairing);
	}
	element_init_G1(skID->y0, pairing);
	for(i = 0; i < LEVEL * 2; i++){
		element_init_G1(skID->y[i], pairing);
	}
	element_init_G1(skID->z, pairing);
	return skID;
}

void setup(param *p, pairing_t pairing, element_t* master_key){

	clock_t start, end;
	int i;

	element_t alpha;
	element_t temp1;

	//start = clock();

	element_init_Zr(alpha, pairing);

	element_random(p->g);

	for(i=0;i<LEVEL;i++){
		element_random(p->h0[i]);
		element_random(p->h1[i]);
		element_random(p->k0[i]);
		element_random(p->k1[i]);
	}
	element_random(p->h_zero);
	element_random(p->k_zero);
	element_random(alpha);
	element_pow_zn(*master_key, p->g, alpha);

//	element_init_G1(p->g, pairing);
	element_init_GT(temp1, pairing);
	pairing_apply(temp1, p->g, p->g, pairing);
	element_pow_zn(p->omega, temp1, alpha);
	//end = clock();
	element_clear(alpha);
}

SKID* keyGen(element_t master_key, param *p, pairing_t pairing, int id){
	SKID* skID;
	int i, j;

	element_t temp, temp1, temp2;
	element_t r;
	element_t alpha_omega;

	element_init_G1(temp, pairing);
	element_init_G1(temp1, pairing);
	element_init_G1(temp2, pairing);
	element_init_Zr(r, pairing);
	element_init_Zr(alpha_omega, pairing);

	skID = init_SKID(id, pairing);

	element_random(r);
	element_random(alpha_omega);

	//g^alpha / g^alpha_omega
	//temp holds g^alpha_omega
	element_pow_zn(temp, p->g, alpha_omega);
	element_div(temp1, master_key, temp);
	//H(ID)^r
	element_set(temp2, p->h_zero);
	for(i = 0; i < LEVEL; i++){
		if(isBit(id, i)){
			element_mul(temp2, temp2, p->h1[i]);
		}
		else{
			element_mul(temp2, temp2, p->h0[i]);	
		}
	}
	element_pow_zn(temp2, temp2, r);
	//x_0
	element_mul(skID->x0, temp1, temp2);
	//x_1~x_l
	for(i = 0; i < LEVEL; i++){
		if(isBit(id, i)){
			element_pow_zn(skID->x[i], p->h0[i], r);
		}
		else{
			element_pow_zn(skID->x[i], p->h1[i], r);
		}
	}
	//y_0
	element_pow_zn(skID->y0, p->k_zero, r);
	//y_1~y_2l
	for(i = 0; i < LEVEL; i++){
		if(isBit(id, i)){
			element_pow_zn(skID->y[i * 2], p->k0[i], r);
			element_mul(skID->y[i * 2], skID->y[i * 2], temp);
			element_pow_zn(skID->y[i * 2 + 1], p->k1[i], r);
		}
		else{
			element_pow_zn(skID->y[i * 2], p->k1[i], r);
			element_mul(skID->y[i * 2], skID->y[i * 2], temp);
			element_pow_zn(skID->y[i * 2 + 1], p->k0[i], r);
		}
	}
	//z=g^r
	element_pow_zn(skID->z, p->g, r);

	skID->id = id;
	element_clear(temp);
	element_clear(temp1);
	element_clear(temp2);
	element_clear(alpha_omega);
	element_clear(r);
	return skID;
}

CT encrypt(param *p, pairing_t pairing, SET S, unsigned char* msg){
	CT ct;
	int i;
	element_t temp;
	element_t M;
	element_t t;

	element_init_G1(temp, pairing);
	element_init_GT(M, pairing);
	element_init_GT(ct.C0, pairing);
	element_init_G1(ct.C1, pairing);
	element_init_G1(ct.C2, pairing);
	element_init_G1(ct.C3, pairing);
	element_init_Zr(t, pairing);

	element_random(t);

	//C0 = omega^t * M
	element_pow_zn(ct.C0, p->omega, t);
	element_from_bytes(M, msg);
	element_to_bytes(msg, M);

	element_mul(ct.C0, ct.C0, M);
	//C1 = g^t
	element_pow_zn(ct.C1, p->g, t);
	//C2 = H(GL)^t
	element_set(temp, p->h_zero);
	for(i = 0; i < LEVEL; i++){
		if(S.c[i] == 0){
			element_mul(temp, temp, p->h0[i]);
		}
		else if(S.c[i] == 1){
			element_mul(temp, temp, p->h1[i]);
		}
		else if(S.c[i] == STAR){
			element_mul(temp, temp, p->h0[i]);
			element_mul(temp, temp, p->h1[i]);
		}
	}
	element_pow_zn(ct.C2, temp, t);
	//C3 = K(ML)^t
	element_set(temp, p->k_zero);
	for(i = 0; i < LEVEL; i++){
		if(S.d[i] == 0){
			element_mul(temp, temp, p->k0[i]);
		}
		else if(S.d[i] == 1){
			element_mul(temp, temp, p->k1[i]);
		}
	}
	element_pow_zn(ct.C3, temp, t);

	element_clear(temp);
	element_clear(M);
	element_clear(t);
	return ct;
}

unsigned char* decrypt(SKID *skID, SET S, CT ct, pairing_t pairing){
	unsigned char *msg;
	int len;
	int d;
	int i, j;
	element_t temp1, temp2;
	element_t A, B, M;
	element_t xprime, yprime;

	element_init_GT(temp1, pairing);
	element_init_GT(temp2, pairing);
	element_init_G1(A, pairing);
	element_init_G1(B, pairing);
	element_init_GT(M, pairing);
	element_init_G1(xprime, pairing);
	element_init_G1(yprime, pairing);
//	element_init_G1(ct.C1, pairing);
//	element_init_G1(skID->z, pairing);

	//finding d; number of bits which in ID is different from ML
	d = 0;
	for(i = 0; i < LEVEL; i++){
		if(isBit(skID->id,i) != S.d[i] &&  S.d[i]!=STAR) {
		//if((isBit(skID->id, i) && S.d[i] == 0) || !(isBit(skID->id, i) && S.d[i] == 1)){
			d++;
		}
	}
	//x'=x0*PI(GL_i=*){x_i}
	element_set(xprime, skID->x0);
	for(i = 0; i < LEVEL; i++){
		if(S.c[i] == STAR){
			element_mul(xprime, xprime, skID->x[i]);
		}
	}
	//y'=(y0*PI(ML!=*){y_2i-1 + ML_i})^d^-1
	element_set(yprime, skID->y0);
	for(i = 0; i < LEVEL; i++){
		if(S.d[i] != STAR)
			element_mul(yprime, yprime, skID->y[i * 2 + S.d[i]]);
		/*
		if(S.d[i] == 0){
			element_mul(yprime, yprime, skID->y[i * 2 - 1]);
		}
		else if(S.d[i] == 1){
			element_mul(yprime, yprime, skID->y[i * 2]);
		}
		*/
	}
	//^d^-1
	element_t d_inv;
	element_init_Zr(d_inv, pairing);
	element_set_si(d_inv, d);
	element_invert(d_inv, d_inv);
	element_pow_zn(yprime, yprime, d_inv);

	//C0 * e(x' * y', C1)^-1 * e(C2 * C3^d^-1, z)
	//temp1=e(x'*y',C1)
	element_mul(A, xprime, yprime);
	pairing_apply(temp1, A, ct.C1, pairing);
	element_pow_zn(B, ct.C3, d_inv);
	element_mul(B, B, ct.C2);
	//temp2=e(C2*C3^d^-1,z)
	pairing_apply(temp2, B, skID->z, pairing);
	//M=C0/temp1*temp2
	element_div(M, ct.C0, temp1);
	element_mul(M, M, temp2);
	
	len = element_length_in_bytes(M);
	msg = (unsigned char*)malloc(sizeof(unsigned char) * (len + 1));
	msg[len] = 0;
	element_to_bytes(msg, M);
	element_clear(temp1);
	element_clear(temp2);
	element_clear(A);
	element_clear(B);
	element_clear(M);
	element_clear(xprime);
	element_clear(yprime);
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
	SKID *sk, *mk, *sk2;
	SKID *testSK[TESTPARAM];
	CT ct;
	unsigned char *c;
	param *p;
	unsigned char msg[] = "hello world!";
	unsigned char msg2[] = "hello2";
	SET S;
	time_t start;
	FILE *fp;

	fp = fopen("element", "w");


	count = fread(params, 1, 1024, stdin);
	if (!count) pbc_die("input error");
	pairing_init_set_buf(pairing, params, count);

	p = init_param(pairing);
	element_init_G1(master_key, pairing);
	setup(p, pairing, &master_key);
	element_out_str(fp, 32, master_key);
	sk = keyGen(master_key, p, pairing, 2);
	//sk2 = keyGen(master_key, p, pairing, 4);

	/*
	for(i = 0 ; i < TESTPARAM; i++){
		testSK[i] = keyGen(master_key, p, pairing, i);
	}
	*/

	// ***** - 0**0*
	for(i=0;i<LEVEL;i++){
		S.c[i] = STAR;
		S.d[i] = STAR;
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
		printf("msg %d: %s\n", getBit(i), c);
	return 0;

}

