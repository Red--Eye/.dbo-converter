.dbo-converter
==============

The Dark Basic Object format has been first described by TheGameCreators in 2003 and their products (FPS Creator & Dark Basic / Dark Basic Pro) have been supporting it till this point. Sadly there are not alot of (good) converters out there. 
I started a very small .dbo converter. It reads all .dbo file data into it's memory and currently is able to output it into a .irrmesh format (still wip).
Hopefully by making this code public, future contributions could lead in support for more formats and code improvement.

It works through a command line interface, by providing two parameters: source destination. 
For example: object.dbo object.irrmesh.