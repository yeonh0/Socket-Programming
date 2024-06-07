#include "recommend_topic.h"

const char* topics[][3] = {
    {"What is your favorite book?", "당신이 가장 좋아하는 책은 무엇인가요?", "¿Cuál es tu libro favorito?"},
    {"What do you like to do in your free time?", "여가 시간에 무엇을 하시나요?", "¿Qué te gusta hacer en tu tiempo libre?"},
    {"What is your favorite movie?", "가장 좋아하는 영화는 무엇인가요?", "¿Cuál es tu película favorita?"},
    {"What is your favorite food?", "가장 좋아하는 음식은 무엇인가요?", "¿Cuál es tu comida favorita?"},
    {"What places would you like to visit?", "어디를 여행하고 싶으신가요?", "¿Qué lugares te gustaría visitar?"},
    {"What are your hobbies?", "당신의 취미는 무엇인가요?", "¿Cuáles son tus pasatiempos?"},
    {"What is your favorite music genre?", "가장 좋아하는 음악 장르는 무엇인가요?", "¿Cuál es tu género de música favorito?"},
};
const int num_topics = sizeof(topics) / sizeof(topics[0]);

const char* recommend_topic(int language_index) {
    srand(time(NULL));
    int index = rand() % num_topics;
    return topics[index][language_index];
}