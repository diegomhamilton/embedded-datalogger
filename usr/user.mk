USERLIB = ./usr

# Required include directories
USERINC =  $(USERLIB) \
           $(USERLIB)/inc
           
# List of all the Userlib files
USERSRC =  $(USERLIB)/src/logger.c
USERSRC +=  $(USERLIB)/src/logger_analog_ch.c
USERSRC +=  $(USERLIB)/src/logger_timing.c
