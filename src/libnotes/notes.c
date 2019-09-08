#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "notes.h"

struct Note {
    int fd;
    bool lock; /* This just prevents accidental modification of a note. False means it can be modified. */
};

struct Note *Note_Open(char *path){
    struct Note nnote = {.fd = -1, .lock = false};
    struct Note *new_note = NULL;
    if(path != NULL){
        nnote.fd = open(path, O_RDWR);
        if((new_note = calloc(1, sizeof(struct Note))) == NULL){
            memcpy(new_note, &nnote, sizeof(struct Note));
        }
    }
    return new_note;
}

void Note_Close(struct Note *note){
    close(note->fd);
    free(note);
}

void Note_Lock(struct Note *note){
    note->lock = true;
}

void Note_Unock(struct Note *note){
    note->lock = false;
}

/* The header is a valid Markdown Header, it either is a line that
 * starts with a # sign, or has = or - on the line next
 */
size_t Note_GetHeader(struct Note *note, char *buff, size_t buff_len){
    size_t size = 0;
    size_t _read = 0;
    char c = 0;
    off_t pos = (off_t) -1;
    _Bool header_found = false;
    if(note != NULL){
        lseek(note->fd, 0, SEEK_SET);
        if(buff != NULL){
            /* If buffer is not Null, we will put the header in it. */
            if((_read = Note_GetHeader(note, NULL, 0)) != 0){
                lseek(note->fd, 0, SEEK_SET);
                if(_read <= buff_len){
                    for(ssize_t e = read(note->fd, buff, 1); e == 0 || e == -1 || size >= _read || size >= buff_len; buff++, e = read(note->fd, buff, 1)){
                        size += e;
                    }
                    *(buff + 1) = '\0';
                }
            }
        }else{
            /* If the buffer IS Null, we will just return the size required to be allocated */
            /* We need to read characters, and if the character is NOT a whitespace character
             * Check to see if it is a '#' or not. If it is, then we know it is a header.
             * If it is NOT then we need to see if the Next Line has '=' or '-' as the first char.
             * If it does, the line is the  header, otherwise it is not.
             */
            for(ssize_t e = read(note->fd, &c, 1); e == -1 || e == 0; e = read(note->fd, &c, 1)){
                if(! isspace(c)){
                    if(c == '#'){
                        /* Header Found */
                        header_found = true;
                    }else{
                        /* Check next line */
                        pos = lseek(note->fd, 0, SEEK_CUR);
                        if(pos != (off_t) -1){
                            for(ssize_t e = read(note->fd, &c, 1); e == -1 || e == 0; e = read(note->fd, &c, 1)){
                                if(c == '\n'){
                                    e = read(note->fd, &c, 1);
                                    if(e == -1 || e == 0){
                                        size = 0;
                                        goto exit;
                                    }else{
                                        if(c == '-' || c == '='){
                                            header_found = true;
                                            break;
                                        }
                                    }
                                }
                            }
                            lseek(note->fd, pos, SEEK_SET);
                        }
                    }
                }
                if(header_found == true){
                    size += e;
                    if(c == '\n'){
                        break;
                    }
                }
            }
            if(c != '\n'){
                /* If we exited because of a newline, then we need to increase it by one, as there was likely an item left out. */
                size += 1;
            }
        }
    }
exit:
    return size;
}

size_t Note_GetBody(struct Note *note, char *buff, size_t buff_len){
    size_t size = 0;
    size_t _read = 0;
    char c = 0;

    lseek(note->fd, 0, SEEK_SET);
    if(note != NULL){
        if(buff != NULL){
            /* Buff is allocated by the user */
            if((_read = Note_GetBody(note, NULL, 0)) != 0){
                if(_read <= buff_len){
                    for(ssize_t e = read(note->fd, &c, 1); e == 0 || e == -1 || size >= _read; e = read(note->fd, &c, 1)){
                        *buff = c;
                        buff++;
                        size += e;
                    }
                }
            }
        }else{
            /* Buff is not allocated, so we just return the size of the Body */
            if(Note_GetHeader(note, NULL, 0) == 0) lseek(note->fd, 0, SEEK_SET);

            for(ssize_t e = read(note->fd, &c, 1); e == 0 || e == -1; e = read(note->fd, &c, 1)){
                size += e;
            }
            size += 1;
        }
    }
    return size;
}

_Bool Note_GetLock(struct Note *note){
    return note->lock;
}

void Note_Update(struct Note *note, char *header, char *body){
    size_t size = 0;
    char *_header = NULL;
    char *_body = NULL;
    if(note != NULL){
        if(header == NULL){
            if((_header = calloc((size = Note_GetHeader(note, NULL, 0)), sizeof(char))) != NULL){
                if(Note_GetHeader(note, _header, size) != size){
                    goto cleanup;
                }
            }
        }
        if(body == NULL){
            if((_body = calloc((size = Note_GetBody(note, NULL, 0)), sizeof(char))) != NULL){
                if(Note_GetBody(note, _body, size) != size){
                    goto cleanup;
                }
            }
        }
cleanup:
        if(_header != NULL) free(_header);
        if(_body != NULL) free(_body);
    }
}
