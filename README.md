# Multi Object Tracker in Qt
This is a library for tracking multi objects in an N-dimentional space in Qt. The library is structured as described in the [MATLAB help document](https://www.mathworks.com/help/fusion/ug/introduction-to-multiple-target-tracking.html "MATLAB help document"): There are four fundamental components in the library:
- Gating
- Assignment
- Filter
- Maintenance

![ScreenShot](https://github.com/alihares99/Multiple-Object-Tracker/assets/46823801/5dded432-f5cb-4ecf-99fa-8b77d07bb04c)

The gating procedure is the same as the MATLAB document. The assignment algorithm, however, is completely different from the MATLAB suggestions to opt for better performance and speed of the algorithm. While common assignment algorithms like [GNN](https://www.mathworks.com/help/fusion/ref/trackergnn-system-object.html) try to match the best observation/measurement to the current tracks, our implementation tries to find only one assignment per track. Whenever there is more than one observation/measurement candidate for a track, those measurements are marked ambiguous and none is assigned to the track. The filter implementation is based on the [alpha-beta filter](https://en.wikipedia.org/wiki/Alpha_beta_filter) and the maintenance module is based on the history of the tracks.

We have provided an implementation for all of the components in the library. But thanks to the abstract factory design pattern, you may easily add your own implementation to any of the components in the library without breaking its structure.

## Dependencies
- Eigen C++ library
- QCustomPlot (included in the repository)
