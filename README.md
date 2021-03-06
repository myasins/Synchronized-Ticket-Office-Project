# Synchronized-Ticket-Office-Project
A basic ticket office simulation using POSIX threads&mutex&semaphores

There are some test cases for a better understanding of how it works and input&output format.

There are 3 diﬀerent theater halls in the system with the given available number of seats:
(a) ODA TIYATROSU, 60 seats
(b) USKUDAR STUDYO SAHNE, 80 seats
(c) KUCUK SAHNE, 200 seats

Conﬁguration File
All the information required for the simulation to progress is in the conﬁguration ﬁle. Your
program should take the conﬁguration ﬁle as input.
• The ﬁrst line of the conﬁguration ﬁle represents the Name of the Theater Hall (i.e., OdaTiy-
atrosu, UskudarStudyoSahne, and KucukSahne).
• The second line represents the total number of clients to be simulated.
• The rest of the conﬁguration ﬁle represents the clients. The order of the lines should be
aligned with the order of client thread creation. A particular line includes the following
information about a client that arrives for a reservation: <client name, arrival time, service
time, seat number>. The format of the conﬁguration ﬁle is ﬁxed. For example, there will
be no white-spaces within lines, and there will be always NumberOfClients + 2 lines in the
conﬁguration ﬁle.

Client-Teller Threads
When the program starts executing, the clients start arriving. The tellers are created in advance,
and they are ready before the clients’ arrival.
• The main thread should create the client threads sequentially. In other words, the client
threads are created one by one, without any waiting time in between. However, after creating
a client thread, the client should sleep (or wait) until his/her arrival time.
• In the ticket oﬃce, a newly arriving client should enter the queue and wait until the requests
of all previous clients in the queue are completed, and the client at the head of the queue is
dispatched to the ﬁrst teller that becomes available. Therefore, the queue mechanism of a
ticket oﬃce should be implemented as a FIFO queue. (Hint: The mutex unlock operation
behaves like FIFO queue in Linux and macOS pthread implementation. Therefore, you
don’t need to implement an extra linked list or similar structure to imitate the FIFO queue
functionality.)
• The tellers should serve the clients in order. If all tellers are available, Teller A gets the ﬁrst
client, Teller B gets the second client, and Teller C gets the third client from the head of the
queue.
• After the creation of the client thread and the sleep duration, the client waits for his/her
turn and when it’s his/her turn, the client selects a seat as stated in the conﬁguration ﬁle.
Teller thread handles the request of the client, reserves the seat, and sleeps for the client’s
service time. After sleeping period, client thread exits. If such a seat cannot be reserved, the
client thread exits after sleeping period without reservation. (Teller’s sleeping period can be
considered as ticket preparation time, whether a reservation successful or not Teller thread
sleeps before client’s leave.)
• The main thread waits until no clients are left to be served.


Input & Output
Your code must read the path of the input and output ﬁles from the command line. You should
create a a Makeﬁle to create the executable (cmake ﬁles are not accepted). The name of the
executable should be simulation.o. We will run your code as follows:
> make
> ./simulation.o configuration_path output_path
