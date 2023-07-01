from shapely import geometry
import path

polygon = [(-1571236.8349707182, 8989180.222117377), (1599362.9654156454, 8924317.946336618), (-1653179.0745812152, 8922145.163675062), (-1626237.6614402141, 8986445.107619021)]

Point_X = -1627875.474
Point_Y = 8955472.968

line = geometry.LineString(polygon)
point = geometry.Point(Point_X, Point_Y)
polygon = geometry.Polygon(line)

print(polygon.contains(point))


start=[ 10.0,10.0]
path.Path("M 3.10182,-0.63196 Z").commands()
path.Path("M 52.285447,33.70123 67.679834,17.197335 c 0,0 26.350756,13.86882 26.07338,14.423572 -0.277376,0.554752 -13.013011,9.744705 -13.983828,9.606016 C 78.798569,41.088236 52.285447,33.70123 52.285447,33.70123 Z").commands()
path.Path("M2,5 C2,2 8,2 8,5 Z").commands()
path.Path("M 25,75 Q50,150 75,100 T 150,150 Z").commands()
path.Path("M 25,100 C 25,150 75,150 75,100 S 100,25 150,75 Z").commands() 
path.Path("M 25,50 C 25,100  150,100  150,50 Z").commands()
path.Path("M 50,50 L 100,100 l 25,0 Z").commands()
absCord = path.Path("m 50,50 100,100 l 25,0 z").makeSimpleAbsolute(start).commands().getAbsCoordinates()
print(absCord)