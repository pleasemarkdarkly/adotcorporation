#include <stdio.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/kernel/kapi.h>
#include <cyg/io/io.h>
#include <cyg/infra/diag.h>
#include "mk4.h"
#include <time.h>
#include "DriveInterface.h"

//was 32 * 4096
#define STACK_SIZE (64 * 4096)
static cyg_handle_t thread;
static cyg_thread thread_obj;
static char stack[STACK_SIZE];

//random string generation vars
//#define NUMSONGS 10000
#define NUMSONGS 100
int sl;
char ts[100];

static void test_entry(cyg_addrword_t data); /* main entry point */
void rand_string(int seed);
void run_test();
void retrieve_data();

extern "C" {
void
cyg_user_start(void)
{
    cyg_thread_create(10, test_entry, (cyg_addrword_t)0, "test",
		      (void *)stack, STACK_SIZE, &thread, &thread_obj);
    cyg_thread_resume(thread);
}
}


static void
test_entry(cyg_addrword_t data)
{

	while (WakeDrive() != ENOERR)
	{
		diag_printf("Unable to wake drive\n");
	}

  //makes a playlist or two, messes around and saves it to file.
  run_test();
  retrieve_data();

}


void rand_string(int seed){
  int i;
  //choose length of string, max is 99, with \0
  sl = rand()%50;
  //printf("%d\n", sl);
  //clear temp string
  for (i=0; i<100; ++i)
    ts[i] = '\0';

  //put in random letters, numbers and spaces  
  for (i=0; i<sl; ++i) {
    while(1) {
     ts[i] = (rand()+seed)%123;
      if (ts[i] > 96) //a-z
        break;
      else if (ts[i] > 64 && ts[i] < 91) //A-Z 
        break;
      else if (ts[i] > 47 && ts[i] < 58) //0-9
        break;
      else if (ts[i] == 32)
        break;
    }
  }
  //printf("%s\n",ts);
}

void run_test() 
{
 int i;
  c4_Row r1, r2, r3;
  c4_View sorted, MenwoHats;
  c4_StringProp ti ("title");
  c4_StringProp fn ("filename");
  c4_StringProp ar ("artist");
  c4_StringProp ge ("genre");
  c4_StringProp info ("info");
  c4_Storage storage ("juke.dat", true);
  c4_View v1= storage.GetAs("v1[ti:S, fn:S, ar:S, ge:S, info:S]");

//  cyg_thread_delay(500);
  srand(time(NULL));

  //create random songs and add them to the default view
  for (i=0; i<NUMSONGS; ++i) {
    rand_string(0);
    ti (r1) = ts;
    rand_string(1);
    fn (r1) = ts;
    rand_string(2);
    ar (r1) = ts;
    rand_string(3);
    ge (r1) = ts;
    rand_string(4);
    info (r1) = ts; 
    v1.Add(r1);
  }
    ti(r1) = "Safety Dance";
    fn(r1) = "dev/wm/safety.mp3";
    ar(r1) = "Men Without Hats";
    ge(r1) = "80's Pop";
    v1.Add(r1);

    //get the number of elements in the view
    i = v1.GetSize();
    diag_printf("Number of Elements: %d\n", i);
    //create a sorted view of the songs
    sorted=v1.Sort();
    //search for the last row added in the sorted view
    i=sorted.Search(r1);
    diag_printf("Key value for song in sorted: %d\n", i);
    const char *str= (ti(sorted[i]));
    diag_printf("title of song at %d: %s\n", i, str);
    str=ti(v1[NUMSONGS]);
    diag_printf("title of last song in default view: %s\n", str);

    ti(r1) = "Eat Twinkies";
    fn(r1) = "dev/wm/eattwinkies.mp3";
    ar(r1) = "Men Without Hats";
    ge(r1) = "80's Pop";
    v1.Add(r1); 
    ti(r1) = "Make me a Sandwich";
    fn(r1) = "dev/wm/makesandwich.mp3";
    ar(r1) = "Men Without Hats";
    ge(r1) = "80's Pop";
    v1.Add(r1); 
    ti(r1) = "I'd like to make a paper airplane";
    fn(r1) = "dev/wm/paperairplane.mp3";
    ar(r1) = "Men Without Hats";
    ge(r1) = "80's Pop";
    v1.Add(r1); 
    ti(r1) = "I wish I were a rubber duckie";
    fn(r1) = "dev/wm/Iwish.mp3";
    ar(r1) = "Men Without Hats";
    ge(r1) = "80's Pop";
    v1.Add(r1); ti(r1) = "smack";
    fn(r1) = "dev/wm/smack.mp3";
    ar(r1) = "Men Without Hats";
    ge(r1) = "80's Pop";
    v1.Add(r1);

    //search for artists using select and print 
    diag_printf("Search for artist\n");
    ar(r2) = "Men Without Hats";
    MenwoHats = v1.Select(r2);
    for (i=0; i<MenwoHats.GetSize(); i++) {
      str = ti(MenwoHats[i]);
      diag_printf("Song title: %s\n", str);
    }

    //re-sort with all the songs and search for specifics
    sorted=v1.Sort();
    //search for the last row added in the sorted view
    diag_printf("Search for last added row in sorted view\n");
    ti(r3) = "I'd like to make a paper airplane";
    i=sorted.Search(r3);
    diag_printf("Key value for song in sorted: %d\n", i);
    str= (ti(sorted[i]));
    diag_printf("title of song at %d: %s\n", i, str);
   
    //do a sort on a genre
    diag_printf("sort on genre 80's pop\n");
    c4_Row r4;
    ge(r4) = "80's Pop";
    c4_View pop = v1.Select(r4);
    for (i=0; i<pop.GetSize(); i++) {
      str = ti(pop[i]);
      diag_printf("Song title: %s", str);
      str = ge(pop[i]);
      diag_printf(" Genre: %s\n", str); 
    }

    //look at sorted songs
    diag_printf("sorted songs\n");
    for (i=0; i<sorted.GetSize(); i++) {
      str = ti(sorted[i]);
      diag_printf("Song title: %s\n", str);
  /*    str = fn(v1[i]);
      diag_printf("Filename: %s\n", str);
      str = ar(pop[i]);
      diag_printf("Artist: %s\n", str); 
      str = ge(v1[i]);
      diag_printf("Genre: %s\n", str);
      str = info(pop[i]);
      diag_printf("Info: %s\n", str); 
      */
    }
   storage.Commit();  
}

void retrieve_data() 
{
  c4_StringProp ti("title"), fn("filename"), ar("artist"), ge("genre"), info("info");        
    c4_Storage storage ("juke.dat",true);     
    c4_View v1 = storage.GetAs("v1[ti:S, fn:S, ar:S, ge:S, info:S]");
    const char *str;
    for (int i=0;i<v1.GetSize(); ++i) {
     str = ti(v1[i]);
     diag_printf("Stored Song Title: %s\n", str);
    }
}





