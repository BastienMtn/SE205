# SE205
TPs et Projets de SE205

## TP3
Accès concurrents, création de threads (question 1) utilisation des mutex (question 2 a 4) puis des sémaphores (question 6 a 8).

### Question 1.5
On voit que l'on commence par récupérer l'heure actuelle avec **gettime(tv_time)**, et que l'on attend ensuite un temps relatif (*relative_time*) calculé comme étant la différence entre le temps absolu de la deadline (*absolute_time*) et la valeur du temps actuel. Le problème, si **gettime** n'est pas suivi par **nanosleep**, c'est que le temps perdu entre les deux ne change rien a la valeur du temps relatif (*relative_time*), et on le perd donc. Si 1ns s'écoule entre **gettime** et **nanosleep**, on aura une nanoseconde de retard, et si on attends un délai assez long comme 1seconde, il se répercutera sur le déroulement du programme avec un comportement hasardeux. Ici, on a notamment la fonction **TIMEVAL_TO_TIMESPEC** qui risque de faire apparaître un retard, puisque l'on ne sait pas comment elle est implémentée, chaque ns passée dans cette fonction sera 1ns de retard dans le programme principal.

Nous avons vu a la question précédente qu'avec un **pthred_cond_timedwait()**, nous pouvons faire attendre un thread jusqu'à ce qu'une condition soit vérifiée, ou bien jusqu'a un temps absolu donné en argument. En partant de cela, on voit qu'en donnant un condition pour laquelle on ne fait jamais de **broadcast** ni de **signal**, le thread va attendre jusqu'au temps absolu. C'est donc ce que je propose d'implémenter ici, nous avons d'ailleurs un exemple de cet usage dans la fonction resynchronize donnée plus haut dans le fichier *utils.c*.