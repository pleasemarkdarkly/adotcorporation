#*****************************************************************************
#
# MKARRAY.AWK - Converts the output from "od -t u1 -v" into a C array.
#
# Copyright (c) 2001 Cirrus Logic, Inc.
#
#*****************************************************************************

#
# This code is executed when AWK starts, before any input lines are processed.
#
BEGIN           {
                    #
                    # Print a warning to the file to indicate that the file
                    # should not be edited as it is a build product.
                    #
                    printf("// THIS IS AN AUTOMATICALLY GENERATED ");
                    printf("FILE...DO NOT EDIT!\n");
                    print "";

                    #
                    # Print the C array declaration.
                    #
                    print "char pcBoot[2048] = {";
                }

#
# This code is executed for every line of the input file.
#
                {
                    #
                    # We only want to process lines that have more than one
                    # field in them (the first field is always the file offset,
                    # which we do not put into the C array).
                    #
                    if(NF > 1)
                    {
                        #
                        # Loop through all of the fields on the line, with the
                        # exception of the first field.
                        #
                        for(i = 2; i <= NF; i++)
                        {
                            #
                            # Print this field.
                            #
                            printf "%3d,", $i;
                        }

                        #
                        # Print a newline.
                        #
                        printf "\n";
                    }
                }

#
# This code is executed right before AWK exits, after it has processed all the
# input lines.
#
END             {
                    #
                    # Print the close of the C array.
                    #
                    print "};"
                }
