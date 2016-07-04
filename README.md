B was a C++ application framework for Mac OS X, written on top of the Carbon APIs rather than Cocoa which is the norm nowadays.

I worked on it off and on for a few years. Prior to uploading it to GitHub, I last touched it around 2006. 
The project's *raison d'être* was to allow me to tinker with various concepts that interested me at the time.
For example:

- Applying the ideas of template metaprogramming (as popularized by the book [Modern C++ Design]).
- Design a framework using modern (at the time) concepts such as :
 - Systematic use of exceptions for error detection and propagation
 - Systematic use of the [RAII] pattern for controlling resource lifetime.
- Good support for Apple Events, including recording.
- Some ideas brought over from Cocoa, such as the Undo system.

This project is made available for historical purposes. I'd be very surprised if anything in it compiled.

[Modern C++ Design]: https://en.wikipedia.org/wiki/Modern_C%2B%2B_Design.
[RAII]: https://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization
