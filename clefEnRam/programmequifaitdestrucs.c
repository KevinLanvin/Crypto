#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include "programmequifaitdestrucs.h"



int main(int argc, char* argv []){
	int choice;
	if(argc != 3){
		fprintf(stderr,"Veuillez entrer les deux clefs\n");
		exit(EXIT_FAILURE);
	}

	/* Initialisation de la clef */
	FILE *keyFile = NULL;
	keyFile = fopen("./superclef",  "r");
	unsigned char* key = malloc(KEY_LENGTH*sizeof(char));
	unsigned char* kk = malloc(KEY_LENGTH*sizeof(char));	
	generateKKFrom(argv[1],argv[2],kk);
	/* Test de l'exitence de la clef */
	if (keyFile == NULL){
		/* Si la clef n'existe pas, on la génère et on la stocke sous forme chiffrée */
	    generateKey(key);
		storeKey(key,kk);
	} else {
		/* Si elle existe, on la récupère et on la déchiffre */
		retreiveKey(key,kk);
	}
	fclose(keyFile);
	memset(kk,0x00,KEY_SIZE);
	free(kk);

	/* Initialisation du fichier de cartes bleues */
	int fichier = open("fichierTopSecret", O_RDWR | O_CREAT);
	
	/* Demarrage du prompt */
	char buffer [BUFFER_SIZE+1];
	char** command;
	/* Initialisation des commandes */
	s_cmd cmd_list[CMD_COUNT] = {
		{"add", cmd_add},
  		{"print", cmd_print},
  		{"find", cmd_find},
		{"exit", cmd_exit]};
	while(1) {
		showMenu();
		if((n=read(0,buffer, BUFFER_SIZE))==-1){
			fprintf(stderr,"Error while reading command");
			exit(EXIT_FAILURE);
		}
		buffer[n]='\0';
		command=split(buffer,' ');
		execute(command,fichier,key);
	}
	close(fichier);
	exit(EXIT_SUCCESS);
} 




/* Génère un tableau de 256 bits aléatoires et le stocke dans dest */
void generateKey(unsigned char* dest){
	unsigned char k[KEY_LENGTH];
	// RAND_bytes(k,KEY_LENGTH);
	memset(k,0xAA,KEY_LENGTH);
	memcpy(dest,k,KEY_LENGTH);
}

/* xor de k1 et k2 stocké dans dest */
void generateKKFrom(unsigned char* k1, unsigned char* k2, unsigned char* dest){
	if(strlen(k1)!=KEY_LENGTH || strlen(k2)!=KEY_LENGTH){
		fprintf(stderr,"Taille des clefs invalide");
		exit(EXIT_FAILURE);
	}
	for(int i=0;i<KEY_LENGTH;++i)
		dest[i]=k1[i]^k2[i];
}

/* Stocke la clef chiffrée par encryptKey dans le fichier "superclef" */
void storeKey(unsigned char* key,unsigned char* encryptKey){
	/* Init vector */
	unsigned char iv[AES_BLOCK_SIZE];
	memset(iv, 0x00, AES_BLOCK_SIZE);
	/* Init buffer sortie */
	unsigned char enc_out[KEY_LENGTH];
	/* Chiffrement */
	AES_KEY enc_key;
	AES_set_encrypt_key(encryptKey, KEY_LENGTH*8, &enc_key); //Size in bits
	AES_cbc_encrypt(key, enc_out, KEY_LENGTH, &enc_key, iv, AES_ENCRYPT);
	/* Ecriture dans le fichier */
	int fichier = open("superclef", O_RDWR | O_TRUNC | O_CREAT , S_IRWXU );
	write(fichier,enc_out,KEY_LENGTH);
	close(fichier);	
}

/* Récupère la clef "key" dans le fichier "superclef" et la déchiffre en utilisant decryptKey */
void retreiveKey(unsigned char* key, unsigned char* decryptKey){
	/* On récupère la clef sous forme chiffrée */	
	int fichier = open("superclef", O_RDONLY);
	unsigned char dec_in[KEY_LENGTH];
	read(fichier,dec_in,KEY_LENGTH);
	close(fichier);
	/* Dechiffrage de la clef */
	unsigned char iv[AES_BLOCK_SIZE];
	memset(iv, 0x00, AES_BLOCK_SIZE);
	AES_KEY dec_key;
	AES_set_decrypt_key(decryptKey, KEY_LENGTH*8, &dec_key); // Size of key is in bits
	AES_cbc_encrypt(dec_in, key, KEY_LENGTH, &dec_key, iv, AES_DECRYPT);
}









char **split(const char *str, char sep)
{
    char **array;
    unsigned int start = 0, stop, toks = 0, t;
    token *tokens = malloc((strlen(str) + 1) * sizeof(token));
    for (stop = 0; str[stop]; stop++) {
        if (str[stop] == sep) {
            tokens[toks].start = str + start;
            tokens[toks].len = stop - start;
            toks++;
            start = stop + 1;
        }
    }
    /* Mop up the last token */
    tokens[toks].start = str + start;
    tokens[toks].len = stop - start;
    toks++;
    array = malloc((toks + 1) * sizeof(char*));
    for (t = 0; t < toks; t++) {
        /* Calloc makes it nul-terminated */
        char *token = calloc(tokens[t].len + 1, 1);
        memcpy(token, tokens[t].start, tokens[t].len);
        array[t] = token;
    }
    /* Add a sentinel */
    array[t] = NULL; 
    free(tokens);
    return array;
}




/* Initialise une carte */
s_card init_card(int* id_card, int secret_pin, char* name) {
	struct card mycard;
	memcpy(mycard.idCard,id_card,CARD_ID_LENGTH);
	mycard.pin = secret_pin;
	memcpy(mycard.name,name,CARD_NAME_LENGTH);
	return mycard;
}

/* Affiche une carte */
void show_card(s_card* c) {
	int i=0;
	printf("Id carte : ");
	for(i; i < 4 ; ++i){
		printf("%d",(c->idCard[i]));
		if(i < 3)
			printf("-");
	}
	printf("\tPropriétaire : %s \n",c->name);
	printf("\tNumero secret : %d \n",c->pin);
}

/* Compare les id */
int compareCardsId(char** id1,char** id2){
	for(int i=0; i<CARD_ID_SEGMENT;++i)
		if(c=strcmp(id1,id2)!=0)
			return c;
	return 0;
}




/* Ajoute une card dans le fichier */
void addNewEntry(int file, const unsigned char* key,const s_card* card){
	unsigned char enc_out [sizeof(s_card)];
	unsigned char enc_in [sizeof(s_card)];
	unsigned char iv [AES_BLOCK_SIZE];
	memcpy(enc_in,&card,sizeof(s_card));
	int pos=lseek(file,-16,SEEK_END);
	if(pos<0)
		memset(iv, 0x00, AES_BLOCK_SIZE);
	else
		read(file,iv,AES_BLOCK_SIZE);
	AES_KEY enc_key;
	AES_set_encrypt_key(key, KEY_LENGTH*8, &enc_key); //Size in bits
	AES_cbc_encrypt(enc_in, enc_out, sizeof(s_card), &enc_key, iv, AES_ENCRYPT);
	lseek(file,0,SEEK_END);
	write(file, enc_out, sizeof(s_card));
}

/* Cherche une card dans le fichier */
void findEntry(int file, const unsigned char* key, char** id){
	s_card card;
	unsigned char iv[AES_BLOCK_SIZE];
	memset(iv, 0x00, AES_BLOCK_SIZE);
	AES_KEY dec_key;
	AES_set_decrypt_key(key,KEY_LENGTH*8, &dec_key);
	unsigned char current_block[AES_BLOCK_SIZE];
	unsigned char dec_out[AES_BLOCK_SIZE];
	int c;
	lseek(file,0,SEEK_SET);
	while(c=read(file,current_block,AES_BLOCK_SIZE) != 0){
		AES_cbc_encrypt(current_block,dec_out,AES_BLOCK_SIZE, &dec_key, iv, AES_DECRYPT);
		memcpy(card,dec_out,sizeof(s_card));
		if(int c=compareCardsId(card.idCard,id)==0)
			show_card(&card);
	}
	printf("Card id not found\n");
}





void printFile(int file,const unsigned char* key){
	unsigned char iv[AES_BLOCK_SIZE];
	memset(iv, 0x00, AES_BLOCK_SIZE);
	AES_KEY dec_key;
	AES_set_decrypt_key(key,KEY_LENGTH*8, &dec_key);
	unsigned char current_block[AES_BLOCK_SIZE];
	unsigned char dec_out[AES_BLOCK_SIZE];
	int c;
	lseek(file,0,SEEK_SET);
	while(c=read(file,current_block,AES_BLOCK_SIZE) != 0){
		AES_cbc_encrypt(current_block,dec_out,AES_BLOCK_SIZE, &dec_key, iv, AES_DECRYPT);
		printf("%s\n",dec_out);
	}
}




void showMenu() {
	printf("Commandes :\n\tfind [card-id]\n\tadd [card-id] [name] [pin]\n\tprint");
	printf("$>");
	fflush(stdout);
} 


void exitProgram(unsigned char* key, int file){
	memset(key,0x00,KEY_SIZE);
	free(key);
	close(file);
	exit(EXIT_SUCCESS);
}

void execute(char** command, int fichier,const unsigned char* key){
	int c;
	for(int i=0;i<CMD_COUNT;++i){
		if(c=strcmp(command[0],cmd_list[i]->name)==0){
			cmd_list[i]->function(command,fichier,key);
			return;
		}
	}
	printf("Mauvaise commande\n");
}




void cmd_exit(char** args,int fichier, const unsigned char* key){
	exitProgram(key);
}

void cmd_print(char** args, int fichier, const unsigned char* key){
	printFile(fichier, key);
}

void cmd_add(char ** args, int fichier, const unsigned char* key){
	s_card card;
	char* card_id [CARD_ID_SEGMENT];
	for(int i=0; i<CARD_ID_SEGMENT; ++i)
		card_id[i]=args[i];
	char* name=args[CARD_ID_SEGMENT+1];
	int pin=atoi(args[CARD_ID_SEGMENT+2];
	card={card_id,name,pin};
	addNewEntry(fichier,key,&card);
}

void cmd_find
