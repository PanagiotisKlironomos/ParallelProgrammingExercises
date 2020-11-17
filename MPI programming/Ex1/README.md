**Άσκηση 1**

Σας ζητείται να φτιάξετε ένα MPI πρόγραμμα (σε γλώσσα C και θεωρώντας point-to-point communication), το οποίο, δοθέντος (ως είσοδο) ενός διανύσματος X (μήκους n στοιχείων xi | i=0…n-1), **να υπολογίζει παράλληλα** σε περιβάλλον &#39;p&#39; επεξεργαστών και να τυπώνει στην οθόνη (ως έξοδο) τα ακόλουθα:

**(α)** τη μέση τιμή των στοιχείων του διανύσματος X:

_m_ _= ( __x__ 0_ _+_ _x __1_ _+_ _x__ 2_ _+ … +_ _x __n__ -1__) /_ _n_

**(β)** τη διασποράτων στοιχείων του διανύσματος X:

_var = ((x__0_ _– m)_ _2_ _+ (x__1_ _– m)_ _2_ _+ (x__2_ _– m)_ _2_ _+ … + (x__n-1_ _– m)__2__) / n_

**(γ)**ένα νέο διάνυσμα Δ όπου κάθε στοιχείο δi θα ισούται με την ποσοστιαία σχέση του αντίστοιχου στοιχείου (xi) του διανύσματος Χ με τη διαφορά μεγίστου-ελαχίστου των τιμών όλου του διανύσματος Χ:

_δi_ _= ((__x __i_ _–_ _x__ min__) / (__x __max_ _–_ _x__ min__)) \* 100_

Χρησιμοποιείστε μόνο τις απλές συναρτήσεις τύπου MPI\_Send()/MPI\_Recv() (μην χρησιμοποιήσετε συναρτήσεις συλλογικής επικοινωνίας). Το σύνολο του απαιτούμενου υπολογιστικού φόρτου θα πρέπει να ισοκατανεμηθεί στους &#39;p&#39; επεξεργαστές του παράλληλου περιβάλλοντός σας. Επίσης, κάθε επεξεργαστής θα πρέπει να λαμβάνει (κατέχει) στην τοπική του μνήμη μόνο τα δεδομένα εισόδου που χρησιμοποιεί για τοπικούς (δικούς του) υπολογισμούς. Τις τιμές/στοιχεία &#39;n&#39;, &#39;xi&#39; θα πρέπει να τις δίνει ο χρήστης και να τις διαβάζει αντίστοιχα από την οθόνη ένας από τους &#39;p&#39; επεξεργαστές του παράλληλου περιβάλλοντός σας (π.χ. ο επεξεργαστής &#39;0&#39; τον οποίον για αυτό το λόγο τον θεωρούμε ως τον &#39;κεντρικό&#39; επεξεργαστή του παράλληλου περιβάλλοντος). Τα αποτελέσματα του προγράμματός σας θα πρέπει επίσης να συγκεντρώνονται στο τέλος στον &#39;κεντρικό&#39; αυτό επεξεργαστή και μέσω αυτού να παρουσιάζονται στο χρήστη.

Αναπτύξτε τον κώδικά σας παραμετρικά ώστε να δουλεύει σωστά για οποιονδήποτε αριθμό πολλαπλών επεξεργαστών &#39;p&#39;. Θεωρήστε αρχικά ότι το &#39;n&#39; είναι ακέραιο πολλαπλάσιο του &#39;p&#39;. Στη συνέχεια προσπαθήστε να επεκτείνετε την υλοποίησή σας ώστε να δουλεύει σωστά για οποιονδήποτε συνδυασμό τιμών &#39;n&#39; και &#39;p&#39;.

Προσπαθήστε επίσης να κάνετε το πρόγραμμά σας να δουλεύει (α) με menu επιλογών (π.χ. 1. Συνέχεια – 2. Εξοδος) και (β) επαναληπτικά (δηλαδή να εμφανίζει επαναληπτικά το παραπάνω menu μέχρι να επιλέξει ο χρήστης την επιλογή &#39;εξόδου&#39; από το πρόγραμμα).
Λύση στο e1.c


**Exercise 1**

You are asked to create an MPI program (in C language and considering point-to-point communication), which, given (as input) of a vector X (length n elements xi | i = 0… n-1), to calculate in parallel in a 'p' processor environment and print the following (as output) to the screen:

**(a)** the mean value of the elements of the vector X:

m = (x 0 + x 1 + x 2 +… + x n -1_) / _ n

**(b)** the scattering elements of the vector X:

var = ((x__0 - m) 2 + (x__1 - m) 2 + (x__2 - m) 2 +… + (x__n-1 - m) 2) / n

**(c)** a new vector Δ where each element δi will be equal to the percentage ratio of the corresponding element (xi) of the vector X to the maximum-minimum difference of the values ​​of the whole vector X:

δi = ((x i - x min) / (x max - x min)) * 100

Use only simple MPI_Send () / MPI_Recv () functions (do not use collective communication functions). The total required load should be evenly distributed among the 'p' processors in your parallel environment. Also, each processor should receive (hold) in its local memory only the input data it uses for local (own) calculations. The values ​​/ elements 'n', 'xi' should be given by the user and read respectively from the screen by one of the 'p' processors of your parallel environment (eg the processor '0' which for this reason we consider it as the 'central' processor of the parallel environment). The results of your program should also be collected at the end in this 'central' processor and presented to the user through it.

Expand your code parameterically so that it works properly for any number of 'p' multiple processors. First consider that 'n' is an integer multiple of 'p'. Then try extending your implementation to make it work properly for any 'n' and 'p' value combinations.

Also try to make your program work (a) with options menu (eg 1. Continue - 2. Exit) and (b) iteratively (ie display the above menu repeatedly until the user selects the 'exit' option from the program). Solution in e1.c
