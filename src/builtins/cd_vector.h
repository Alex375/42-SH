#ifndef CD_VECTOR_H
#define CD_VECTOR_H

/*!
 * @brief Struct of vector for the cd function
 */
struct vector
{
    struct string *str;
    struct vector *next;
};

/*!
 * @brief Create a new vector
 */
struct vector *vector_create();

/*!
 * @brief Add at the end of the current vector a new element
 * @param args (vec) The current vector. (data) The string to add at the end of
 * vec
 */
struct vector *vector_init(struct vector *vec, struct vector *next, char *data);

/*!
 * @brief Add at the end of the current vector a new element
 * @param args (vec) The current vector. (data) The string to add at the end of
 * vec
 */
void vector_add(struct vector *vec, char *data);

/*!
 * @brief Remove the last element of the current vector
 * @param args (vec) The current vector
 */
void vector_pop(struct vector *vec);

#endif /* !CD_VECTOR_H */