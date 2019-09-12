#ifndef S_NOTES
#define S_NOTES

/**
 * @file notes.h
 *
 * @brief A simple library for sticky-notes.
 *
 * This is just a simple library for managing sticky-notes and the like.
 * @author Sapeint/Chanku
 * @copyright MIT License
 * @version 1.0.0
 */

struct Note;
typedef struct Note Note;

/**
 * Opens a Note at a given path.
 * @param note_path The file path to the note file.
 * @returns A Note Pointer or NULL if there is an error.
 * @note If it returns NULL errno is set to an error associated with POSIX read()
 */
struct Note *Note_Open(char *note_path);

/**
 * Closes an already-opened note.
 * @param note The note to close.
 */
void Note_Close(struct Note *note);

/**
 * Updates a Note on the File-System.
 *
 * @param note The note to be updated.
 * @param header the header of the note. This can be NULL.
 * @param body the body of the note. This can be NULL.
 *
 * @note Any NULL values passed means use the current value.
 */
void Note_Update(struct Note *note, char *header, char *body);

/**
 * Marks a note as 'Locked'.
 * @param note The note to be locked.
 * @note Locking doesn't have any inherent meaning within the library.
 */
void Note_Lock(struct Note *note);

/**
 * Marks a note as 'Unlocked'.
 * @param note The note to be unlocked.
 * @note Unlocking doesn't have any inherent meaning within the library.
 */
void Note_Unock(struct Note *note);

/**
 * Gets the Header of a note on the disk.
 * If buff is NULL, then it just returns the size of the Header.
 *
 * @param note The note you wish to use.
 * @param[out] buff The buffer to output the data to. This may be NULL
 * @param buff_len The size of the buffer.
 * @returns The size of the Header.
 */
size_t Note_GetHeader(struct Note *note, char *buff, size_t buff_len);

/**
 * Gets the Body of an opened note.
 * If buff is NULL, then you will get the size of the body.
 *
 * @param note The note you wish to use.
 * @param[out] buff the buffer to outtput the data to. This may be NULL.
 * @param buff_len The size of the buffer.
 * @returns The size of the Body.
 */
size_t Note_GetBody(struct Note *note, char *buff, size_t buff_len);

/**
 * Gets the current Lock status of an opened note.
 *
 * @param note the note you wish to use.
 * @note Locking doesn't have any inherent meaning within the library.
 */
_Bool Note_GetLock(struct Note *note);
#endif
