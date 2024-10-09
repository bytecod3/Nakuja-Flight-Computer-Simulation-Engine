x = 1:1:5;
y = [x;rand(1,5)];
fileID = fopen( 'nums2.txt' , 'w' );
fprintf(fileID, '%d %4.4f\n' ,y);

fprintf(fileID, '%d %4.4f\n', [34 45]);
fprintf(fileID, '%d %4.4f\n', [100 677]);
fclose(fileID);