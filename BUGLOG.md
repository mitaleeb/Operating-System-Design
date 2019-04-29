ISSUE BOARD
-----

## Issue: File System read dentry functions memcpy can't check if successful

Date Created: March 13, 2019

Description: In read_dentry_by_index and read_dentry_by_name, there is no check of memcpy to make sure they are successfuly before returning 0 (success). memcpy doesn't seem to return number of bytes copied and just return a pointer to the destination so we are not checking if the memcpy is successful. 

Resolution Summary:
Use strcmp function to check strings and if result is 0, the strings are the same in read_dentry_by_name. This check makes sure that we will do a successful memcpy of strings into the dentry. 
In read_dentry_by_index, there is a check of the index parameter before doing memcpy to ensure success 


##Issue: Modify RTC/ Terminal functions to follow same parameter format

Date Created: March 26, 2019

Description: Terminal, RTC, Keyboard and file system should all follow the same parameter format and currently it is inconsistent. 
This makes it more difficult when a general system call is developed in Checkpoint 3/4. 

Resolution Summary: System calls and drivers follow correct parameter format as shown below:
int32 t read (int32 t fd, void* buf, int32 t nbytes);
int32 t write (int32 t fd, const void* buf, int32 t nbytes);
int32 t open (const uint8 t* filename);
int32 t close (int32 t fd);


##Issue: Directory open/close/read/write'ls' test case does not print '.'

Date Created: March 26, 2019

Description: Fix Directory File List Output to ensure the '.' is included on output. Contents are outputted but '.' is missing. 

Resolution Summary: 
Call to dir_read for failure case caused the first iteration of ls output to be skipped. Then the '.' is skipped over if dir_open and dir_read is successful after doing a failure check on the first iteration. Modifying test case order so calling dir_open before the success call to dir_read gave the correct output!


##Issue: Terminal "CTRL + L" buffer for text does not fill whole line

Date Created: March 26, 2019

Description: Terminal "CTRL + L" buffer for text should fill hole line. Right now, if we hit "CTRL +L" after typing some text, it assumes it is in the buffer and so we cannot type text to fill up the whole line because it auto jumps to the next line assuming there is still characters in the buffer.

Resolution Summary: When "CTRL + L" is pressed, set screen_x and screen_y back to zero and update term_buffer_index back to zero so that it writes to the beginning of the buffer rather than where the last buffer left off. 


##Issue: Cannot read large file size and garbage is output

Date Created: March 26, 2019

Description: File size content is read by limitation of BLOCK_SIZE. This should be extended to be able to read larger files and output to screen. Need to fix fs.c and add specific test case for large file size.

Resolution Summary: Made array size max size of file contents rather than BLOCK_SIZE. The larger array should be large enough for all files in the fs at the moment.


##Issue: Cannot read binary files in file_read

Date Created: March 26, 2019

Description: Currently, binary files are only being read by one line but whole thing is not outputted to screen. It is tied to the large file size but also as soon as a NULL character is found, the current program terminates the output. Thus, the whole binary file is not output to terminal screen.

Resolution Summary: Made array size max size of file contents in terminal read so that the file is not NULL terminated or done reading after max size is reached. This way whole file can be output. 

##Issue: No error handling in directory open/close

Date Created: March 26, 2019

Description: No check to see if fname exists in the directory for open and close functions. 

Resolution Summary: Check if "fname" exists before return value and rest of function.


##Issue: read_data/ file_read needs to return bytes read on success

Date Created: March 26, 2019

Description: The Bytes read right now seems to always be some mod factor of BLOCK_SIZE and does not effectively return the bytes read (according to TA). Looking at frame0.txt in the test file output the whole file correctly but printf for the return value of file_read which calls read_data returns 0. 

Resolution Summary: Check if the size of file length is bigger than one block. If it is then we read the whole block plus the offset into the next block and make sure this is added to the bytes read. Before, it would return bytes strictly based on how many whole number blocks were used in the file. 

##Issue: Terminal write does not return correct length 

Date Created: March 26, 2019

Description: The first three worked correctly but the last one should output 3 not 4 because the string is only length 3. 
terminal_write("aaa", 2);
terminal_write("bbb", -1);
terminal_write("ccc", 3);
terminal_write("ddd", 4);

Resolution Summary: Added these test cases into the test file and changed terminal_write so that if the given length to copy is larger than the actual string length, the returned output is the correct string length. 


##Issue: Setting stdin and stdout to being read-only and write-only, respectively

Date Created: April 7, 2019

Description: I think this was mentioned somewhere is the documentation that we need to do this, but I'm unsure how.  I was thinking maybe just setting the write for stdout_fops and the read for stdin_fops to NULL, but maybe that doesn't work since we'll try to do a null function call. 

Resolution Summary: Completed by setting the fops for stdin and stdout write and read, respectively, to garbage_read/garbage_write.


##Issue: Halt cascades into many halts after the second "exit" from the furthest shell, in cascaded shells.

Date Created: April 7, 2019

Description: Steps to reproduce: (if max process is more than 2)

start the os
shell should execute automatically (nothing needs to be done in this step)
type "shell" and enter
type "shell" and enter
type "shell" and enter
type "exit" and enter
type "exit" and enter

The halts now cascade and every process closes, restarting us into a fresh shell at PID 0.

Resolution Summary: Fixed by moving the saved process's esp and ebp into the current process's control block (other than parent_pcb, each pcb block also holds parent_esp and parent_ebp). This ensures that after our second shell, we still have all the data to backtrack to the first shell.

##Issue: FISH and PINGPONG always write to active terminals

Date Created: April 20, 2019

Description: This is a problem with how the user-level programs write to video memory (they ignore the terminal struct's video memory structure). Can probably be fixed with the current paging functions. Video memory for paging is being mapped backwards - since the video card uses the physical address 0xB8000, we can't be mapping that out of the range of video memory. Instead, we can use three separate pages and then copy the video memory into the relevant page and vice versa. 

Resolution Summary: See issue "terminal_read is giving data to whichever the current running process is". 

##Issue: Executing processes with correct parent processes does not work (execute/pcb data for each terminal issues)

Date Created: April 20, 2019

Description: We don't actually update the current processes esp/ebp in the case that it is the first process of any of the terminals. It works when we are executing but when we try to do a context switch we get screwed. Somehow we need to make sure the esp and ebps are saved initially, before we do the first context switch (this causes a page fault). 

Resolution Summary: The fix for this (since it only happens in the case where we're initializing one of the three initial shells) is to just handle the context switching when we launch the terminals themselves. Tried root pcb to connect all three shells to one "invisible" process but decided against this because they are separate terminals. 


##Issue: FISH shows only black

Date Created: April 20, 2019

Description: Video memory is initialized to be black color, we need to change it when creating new terminals/video pages so that it is initialized to either white or a different color for fish to work properly. 

Resolution Summary: The fix for this (since it only happens in the case where we're initializing one of the three initial shells) is to just handle the context switching when we launch the terminals themselves. Tried root pcb to connect all three shells to one "invisible" process but decided against this because they are separate terminals. We changed the colors of each terminal and were able to get fish to display correctly. 

## Issue: Cannot type half a command and then switch terminals, switch back, and finish the command.

Date Created: April 22, 2019

Description: Steps to reproduce:
Type "testp" in one terminal, switch to another
type "testprint" and enter, let testprint run
switch back to the first terminal, type "rint" and enter.
Expected behavior: testprint runs in the first terminal
Current behavior: fails to run testprint after switching 


Resolution Summary: The fix for this is saving each terminal's buffer even after switching to a different visible terminal so that if you switch back, the buffer is saved and finishing the rest of the string command restores the full string and executes testprint correctly. 

##Issue: terminal_read is giving data to whichever the current running process is

Date Created: April 23, 2019

Description: When terminal_read is called, it gives whatever is in the generic old_term_buffer to the current active process. This can cause issues in which a process active on terminal 2 (for example) gets the keyboard input from the user while the user is typing into terminal 1. 

Resolution Summary: Modified terminal read and write as well as putc/printf such that the curr_pcb stores the values of whether or not the current index is the visible terminal and modifies background terminals given curr_pcb->term_index. This way each process is not always writing to visible terminal or the process that was most recently started (usually the visible terminal shell F3). 


##Issue: Pits interrupts not interrupting after the first interrupt

Date Created: April 24, 2019

Description: The behavior seen is that the pit interrupts are correctly set up in the IDT (presumably), since they execute once, but once we actually get to the shell none of the pit interrupts happen.

Resolution Summary: Fixed by finding a bug in disable_irq in i8259.c. The disable irq did not have the proper bit shifting to ensure that the correct irq was disabled. As a result, when we disabled RTC or Keyboard, PIT was also being disabled. However, enable_irq was correct, so once we disabled either rtc or keyboard, PIT was never re-enabled.

##Issue: If running multiple processes, they slow down a significant amount.

Date Created: April 24, 2019

Description: We need to virtualize the RTC because currently RTC interrupts for counter and fish running concurrently really slows down each process. 


Resolution Summary: The easiest way to do this while only modifying the RTC files is doing this:

1) change interrupt_flag to be a ticks variable that counts upwards
2) have rtc read check the difference in the ticks variable between when it was called and the loop itself, so that our non-currently-scheduled programs can be responsive to RTC interrupts that happen not during their time-slice.
3) make sure to have logic that resets the ticks if they ever overflow.




