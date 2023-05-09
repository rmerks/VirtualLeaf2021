from shapely import geometry

polygon = [(-1571236.8349707182, 8989180.222117377), (1599362.9654156454, 8924317.946336618), (-1653179.0745812152, 8922145.163675062), (-1626237.6614402141, 8986445.107619021)]

Point_X = -1627875.474
Point_Y = 8955472.968

line = geometry.LineString(polygon)
point = geometry.Point(Point_X, Point_Y)
polygon = geometry.Polygon(line)

print(polygon.contains(point))