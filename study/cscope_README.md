# Using Cscope on large projects
## (example: the Linux kernel)

Cscope can be a particularly useful tool if you need to wade into a large code base. You can save yourself a lot of time by being able to do fast, targeted searches rather than randomly grepping through the source files by hand (especially since grep starts to take a while with truly large code base).

## 1. Get the source
## 2. Figure out where you want to put your Cscope database files.
## 3. Generate cscope.files with a list of files to be scanned.

* simple
cscope -R

* For many project, find command may be as simple as

`cd /
find /my/project/dir -name '*.java' > /my/cscope/dir/cscope.files`

* For the Linux kenel, it's a little trickier, since we want to exclude all the code in the docs and scripts directories, plus all of the architectured and assembly code for all chips except for the beloved Intel x86 (which I'm guessing is the architecture you're interested in). Additionally, I'm excluding all kernel driver code in this example (they more than double the amount of code to be parsed, which bloats the Cscope database, and they contain many duplicate definitions, which often makes searching harder. If you are interested in the driver code, omit the relevant line below, or modify it to print out only the driver files you're interested in):

`LNX=/home/jru/linux-2.4.18
cd /
find $LNX
  -path "$LNX/arch/*" ! -path "$LNX/arch/i386*" -prune -o
  -path "$LNX/include/asm-*" ! -path "$LNX/include/asm-i386*" -prune -o
  -path "$LNX/tmp*" -prune -o
  -path "$LNX/Documentation*" -prune -o
  -path "$LNX/scripts*" -prune -o
  -path "$LNX/drivers*" -prune -o
  -name "*.[chxsS]" -print > /home/jru/cscope.files
`

While find commands can be a little tricky to write, for large projects they are much easier than editing a list of files manually, and you can also cut and paste a solution from someone else.

## 4. Generate the Cscope database. Now it's time to generate the Cscope database:

`cd /home/jru/cscope
cscope -b -q -k`

The -b flag tells Cscope to just build the database, and not launch the Cscope GUI. The -q causes an additional, 'inverted index' file to be created, which makes searches run much faster for large databases. Finally, -k sets Cscope's kernel mode--it will not look in /usr/include for any header files that are #included in your source files(this is mainly useful when your are using Cscope with operating system and/or C library source code, as we are here).

## 5. Using the database
