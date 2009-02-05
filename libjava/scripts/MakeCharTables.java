/* MakeCharTables.java - converts gnu.java.lang.CharData into
                         include/java-chartables.h
   Copyright (C) 2002 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */

import gnu.java.lang.CharData;

public class MakeCharTables implements CharData
{
  public static void main(String[] args)
  {
    System.out.println("/* java-chartables.h -- Character tables for java.lang.Character -*- c++ -*-\n"
                       + "   Copyright (C) 2002 Free Software Foundation, Inc.\n"
                       + "   *** This file is generated by scripts/MakeCharTables.java ***\n"
                       + "\n"
                       + "This file is part of GNU Classpath.\n"
                       + "\n"
                       + "GNU Classpath is free software; you can redistribute it and/or modify\n"
                       + "it under the terms of the GNU General Public License as published by\n"
                       + "the Free Software Foundation; either version 2, or (at your option)\n"
                       + "any later version.\n"
                       + "\n"
                       + "GNU Classpath is distributed in the hope that it will be useful, but\n"
                       + "WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                       + "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
                       + "General Public License for more details.\n"
                       + "\n"
                       + "You should have received a copy of the GNU General Public License\n"
                       + "along with GNU Classpath; see the file COPYING.  If not, write to the\n"
                       + "Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA\n"
                       + "02111-1307 USA.\n"
                       + "\n"
                       + "Linking this library statically or dynamically with other modules is\n"
                       + "making a combined work based on this library.  Thus, the terms and\n"
                       + "conditions of the GNU General Public License cover the whole\n"
                       + "combination.\n"
                       + "\n"
                       + "As a special exception, the copyright holders of this library give you\n"
                       + "permission to link this library with independent modules to produce an\n"
                       + "executable, regardless of the license terms of these independent\n"
                       + "modules, and to copy and distribute the resulting executable under\n"
                       + "terms of your choice, provided that you also meet, for each linked\n"
                       + "independent module, the terms and conditions of the license of that\n"
                       + "module.  An independent module is a module which is not derived from\n"
                       + "or based on this library.  If you modify this library, you may extend\n"
                       + "this exception to your version of the library, but you are not\n"
                       + "obligated to do so.  If you do not wish to do so, delete this\n"
                       + "exception statement from your version. */\n"
                       + "\n"
                       + "#ifndef __JAVA_CHARTABLES_H__\n"
                       + "#define __JAVA_CHARTABLES_H__\n"
                       + "\n"
                       + "// These tables are automatically generated by scripts/MakeCharTables.java.\n"
                       + "// This is in turn parsing gnu.java.lang.CharData, which is generated by\n"
                       + "// scripts/unicode-muncher.pl.  The Unicode data comes from\n"
                       + "// www.unicode.org; this header is based on\n"
                       + "// " + SOURCE + ". JDK 1.4 uses Unicode version 3.0.0.\n"
                       + "// DO NOT EDIT the tables.  Instead, fix the upstream scripts and run\n"
                       + "// them again.\n"
                       + "\n"
                       + "// The data is stored in C style arrays of the appropriate CNI types, to\n"
                       + "// guarantee that the data is constant and non-relocatable.  The field\n"
                       + "// <code>blocks</code> stores the offset of a block of 2<supSHIFT</sup>\n"
                       + "// characters within <code>data</code>. The data field, in turn, stores\n"
                       + "// information about each character in the low order bits, and an offset\n"
                       + "// into the attribute tables <code>upper</code>, <code>lower</code>,\n"
                       + "// <code>numValue</code>, and <code>direction</code>.  Notice that the\n"
                       + "// attribute tables are much smaller than 0xffff entries; as many characters\n"
                       + "// in Unicode share common attributes.  Finally, there is a listing for\n"
                       + "// <code>title</code> exceptions (most characters just have the same title\n"
                       + "// case as upper case).\n"
                       + "\n"
                       + "// This file should only be included by natCharacter.cc\n"
                       + "\n"
                       + "/**\n"
                       + " * The character shift amount to look up the block offset. In other words,\n"
                       + " * <code>(char) (blocks[ch >> SHIFT] + ch)</code> is the index where\n"
                       + " * <code>ch</code> is described in <code>data</code>.\n"
                       + " */\n"
                       + "#define SHIFT " + SHIFT);

    convertString("/**\n"
                  + " * The mapping of character blocks to their location in <code>data</code>.\n"
                  + " * Each entry has been adjusted so that a modulo 16 sum with the desired\n"
                  + " * character gives the actual index into <code>data</code>.\n"
                  + " */",
                  char.class, "blocks", BLOCKS);

    convertString("/**\n"
                  + " * Information about each character.  The low order 5 bits form the\n"
                  + " * character type, the next bit is a flag for non-breaking spaces, and the\n"
                  + " * next bit is a flag for mirrored directionality.  The high order 9 bits\n"
                  + " * form the offset into the attribute tables.  Note that this limits the\n"
                  + " * number of unique character attributes to 512, which is not a problem\n"
                  + " * as of Unicode version 3.2.0, but may soon become one.\n"
                  + " */",
                  char.class, "data", DATA);

    convertString("/**\n"
                  + " * This is the attribute table for computing the numeric value of a\n"
                  + " * character.  The value is -1 if Unicode does not define a value, -2\n"
                  + " * if the value is not a positive integer, otherwise it is the value.\n"
                  + " */",
                  short.class, "numValue", NUM_VALUE);

    convertString("/**\n"
                  + " * This is the attribute table for computing the uppercase representation\n"
                  + " * of a character.  The value is the difference between the character and\n"
                  + " * its uppercase version.\n"
                  + " */",
                  short.class, "upper", UPPER);

    convertString("/**\n"
                  + " * This is the attribute table for computing the lowercase representation\n"
                  + " * of a character.  The value is the difference between the character and\n"
                  + " * its lowercase version.\n"
                  + " */",
                  short.class, "lower", LOWER);

    convertString("/**\n"
                  + " * This is the attribute table for computing the directionality class\n"
                  + " * of a character.  At present, the value is in the range 0 - 18 if the\n"
                  + " * character has a direction, otherwise it is -1.\n"
                  + " */",
                  byte.class, "direction", DIRECTION);

    convertString("/**\n"
                  + " * This is the listing of titlecase special cases (all other character\n"
                  + " * can use <code>upper</code> to determine their titlecase).  The listing\n"
                  + " * is a sequence of character pairs; converting the first character of the\n"
                  + " * pair to titlecase produces the second character.\n"
                  + " */",
                  char.class, "title", TITLE);

    System.out.println();
    System.out.println("#endif /* __JAVA_CHARTABLES_H__ */");
  }

  private static void convertString(String header, Class type,
                                    String name, String field)
  {
    System.out.println();
    System.out.println(header);
    System.out.println("static const j" + type.getName() + " " + name
                       + "[] = {");
    char[] data = field.toCharArray();
    int wrap;
    if (type == char.class)
      wrap = 10;
    else if (type == byte.class)
      wrap = 21;
    else if (type == short.class)
      wrap = 13;
    else
      throw new Error("Unexpeced type");
    for (int i = 0; i < data.length; i += wrap)
      {
        System.out.print("   ");
        for (int j = 0; j < wrap; j++)
          {
            if (i + j >= data.length)
              break;
            System.out.print(" ");
            if (type == char.class)
              System.out.print((int) data[i + j]);
            else if (type == byte.class)
              System.out.print((byte) data[i + j]);
            else if (type == short.class)
              System.out.print((short) data[i + j]);
            System.out.print(",");
          }
        System.out.println();
      }
    System.out.println("  };\n"
                       + "/** Length of " + name + ". */\n"
                       + "static const int " + name + "_length = "
                       + data.length + ";");
  }
}