#*****************************************************************************
#
# GETDEFS.AWK - Gets the set of defines that configure the software and
#               produces output that can be included by make.
#
# Copyright (c) 2001 Cirrus Logic, Inc.
#
#*****************************************************************************

#
# This code is executed when AWK starts, before any input lines are processed.
#
BEGIN                   {
                            #
                            # Start by printing a message indicating that the
                            # output file is an automatically generated file
                            # therefore should not be edited.
                            #
                            printf("# THIS IS AN AUTOMATICALLY GENERATED ");
                            printf("FILE...DO NOT EDIT!\n");
                            print "";

                            #
                            # We start with no symbols defined.
                            #
                            numdefs = 0;

                            #
                            # We start by including definitions (until we
                            # encounter an ifdef).
                            #
                            include = 1;

                            #
                            # See if there is a symbol defined on the command
                            # line.
                            #
                            if(ARGV[1] == "-D")
                            {
                                #
                                # Add this symbol to our set of defined
                                # symbols, with no value.
                                #
                                defs[0] = ARGV[2];
                                vals[0] = "";

                                #
                                # Increment the count of symbols in the table.
                                #
                                numdefs++;

                                #
                                # Remove these arguments from the command line
                                # (so that awk does not try to read from files
                                # by those names).
                                #
                                ARGV[1] = "";
                                ARGV[2] = "";
                            }
                        }

#
# This code is executed when we find a "#ifdef" at the beginning of a line.
#
/^\#ifdef/              {
                            #
                            # Loop through all of the symbols that are
                            # currently defined.
                            #
                            for(i = 0; i < numdefs; i++)
                            {
                                #
                                # See if the symbols match.
                                #
                                if($2 == defs[i])
                                {
                                    #
                                    # The symbols match, so stop looking.
                                    #
                                    break;
                                }
                            }

                            #
                            # See if we found a symbol that matched.
                            #
                            if(i == numdefs)
                            {
                                #
                                # No symbol matched, so loop through all of the
                                # symbols that are currently defined.
                                #
                                for(i = 0; i < numdefs; i++)
                                {
                                    #
                                    # See if this symbol is "BOARD" and it's
                                    # values matches the #ifdef.
                                    #
                                    if((defs[i] == "BOARD") &&
                                       (vals[i] == $2))
                                    {
                                        #
                                        # The BOARD symbol value matches, so
                                        # stop looking.
                                        #
                                        break;
                                    }
                                }
                            }

                            #
                            # See if we found a symbol that matched.
                            #
                            if(i == numdefs)
                            {
                                #
                                # We did not find a symbol that matched, so
                                # we should not include further defines until
                                # we find a "#endif".
                                #
                                include = 0;
                            }
                        }

#
# This code is executed when we find a "#endif" at the beginning of a line.
#
/^\#endif/              {
                            #
                            # We always include further defines after a
                            # "#endif" (nested "#ifdef"s are not supported).
                            #
                            include = 1;
                        }

#
# This code is executed when we find a "#define" at the beginning of a line.
#
/^\#define/             {
                            #
                            # See if we should be including defines.
                            #
                            if(include == 1)
                            {
                                #
                                # Add this symbol to our table.
                                #
                                defs[numdefs] = $2;

                                #
                                # See if a value was specified for this define.
                                #
                                if($3 != "")
                                {
                                    #
                                    # A value was specified, so remember it.
                                    #
                                    vals[numdefs] = $3;
                                }
                                else
                                {
                                    #
                                    # No value was specified, so give it the
                                    # default value of "1".
                                    #
                                    vals[numdefs] = "1";
                                }

                                #
                                # Increment the count of symbols in the table.
                                #
                                numdefs++;
                            }
                        }

#
# This code is executed for all other lines.
#
                        {
                        }

#
# This code is executed right before AWK exits, after it has processed all the
# input lines.
#
END                     {
                            #
                            # Loop through all the symbols in our table.
                            #
                            for(i = 0; i < numdefs; i++)
                            {
                                #
                                # Print out this symbol definition if it has a
                                # value.
                                #
                                if(vals[i] != "")
                                {
                                    print defs[i] " = " vals[i];
                                }
                            }
                        }
