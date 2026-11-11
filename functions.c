#include "headers.h"

void help()
{
    printf("\n====================HELP MENU====================\n");
    printf("1. -v -> to view MP3 file contents\n");
    printf("2. -e -> to edit MP3 file contents\n");
    printf("         2.1. -t -> to edit song title\n");
    printf("         2.2. -a -> to edit artist name\n");
    printf("         2.3. -A -> to edit ablum name\n");
    printf("         2.4. -y -> to edit year\n");
    printf("         2.5. -m -> to edit genre\n");
    printf("         2.6. -c -> to edit comment\n");
    printf("==================================================\n");
}

void view(char file[])
{
    FILE *fp;
    fp = fopen(file, "rb");

    if (fp == NULL)
    {
        perror("ERROR");
        return;
    }

    char file_head[4];
    if (fread(file_head, sizeof(char), 3, fp) != 3)
    {
        printf("version not matched\n");
        fclose(fp);
        return;
    }
    file_head[3] = '\0';

    if ((strcmp(file_head, "ID3") != 0))
    {
        printf("Invalid ID3 tag\n");
        fclose(fp);
        return;
    }

    unsigned char file_version[3];
    if (fread(file_version, sizeof(char), 2, fp) != 2)
    {
        printf("Unable to read version\n");
        fclose(fp);
        return;
    }
    file_version[2] = '\0';

    if (file_version[0] != 3 || file_version[1] != 0)
    {
        printf("Invalid version\n");
        fclose(fp);
        return;
    }

    fseek(fp, 1, SEEK_CUR);

    unsigned char header_size[4];
    if ((fread(header_size, sizeof(char), 4, fp)) != 4)
    {
        printf("Unable to read header size\n");
        fclose(fp);
        return;
    }

    int tag_size = 0;
    for (int i = 0; i < 4; i++)
    {
        tag_size = (tag_size << 8) | header_size[i];
    }

    char title[100] = "";
    char artist[100] = "";
    char album[100] = "";
    char year[10] = "";
    char genre[50] = "";
    char comment[100] = "";

    int i = 0;
    while (i < tag_size)
    {
        char frame_id[5];
        if ((fread(frame_id, sizeof(char), 4, fp)) != 4)
        {
            printf("Unable to read Frame id\n");
            break;
        }
        frame_id[4] = '\0';

        if (frame_id[0] == '\0')
        {
            break;
        }

        unsigned char framehead_size[4];
        if ((fread(framehead_size, sizeof(char), 4, fp)) != 4)
        {
            printf("Unable to reaed Frame size\n");
            break;
        }

        int framesize = 0;
        int j;
        for (j = 0; j < 4; j++)
        {
            framesize = (framesize << 8) | framehead_size[j];
        }

        unsigned char frame_flag[2];
        if ((fread(frame_flag, sizeof(char), 2, fp)) != 2)
        {
            printf("Unable to read Frame flag\n");
            break;
        }

        char *data;
        data = malloc(framesize + 1);
        if (data == NULL)
        {
            perror("ERROR");
            fclose(fp);
            return;
        }

        if ((fread(data, sizeof(char), framesize, fp)) != framesize)
        {
            printf("Unable to read data\n");
            free(data);
            fclose(fp);
            return;
        }
        data[framesize] = '\0';

        if ((strcmp(frame_id, "TIT2") == 0))
        {
            snprintf(title, sizeof(title), "%s", data + 1);
        }
        else if ((strcmp(frame_id, "TPE1") == 0))
        {
            snprintf(artist, sizeof(artist), "%s", data + 1);
        }
        else if ((strcmp(frame_id, "TALB") == 0))
        {
            snprintf(album, sizeof(album), "%s", data + 1);
        }
        else if ((strcmp(frame_id, "TYER") == 0))
        {
            snprintf(year, sizeof(year), "%s", data + 1);
        }
        else if ((strcmp(frame_id, "TCON") == 0))
        {
            snprintf(genre, sizeof(genre), "%s", data + 1);
        }
        else if ((strcmp(frame_id, "COMM") == 0))
        {
            snprintf(comment, sizeof(comment), "%s", data + 5);
        }
        free(data);

        i += 10 + framesize;
    }

    printf("======================================================================\n");
    printf("                         MP3 TAG INFORMATION\n");
    printf("======================================================================\n");
    printf("%-15s : %s\n", "Title", title);
    printf("%-15s : %s\n", "Artist", artist);
    printf("%-15s : %s\n", "Album", album);
    printf("%-15s : %s\n", "Year", year);
    printf("%-15s : %s\n", "Genre", genre);
    printf("%-15s : %s\n", "Comment", comment);
    printf("======================================================================\n");
    fclose(fp);
}

void edit(char option[], char value[], char file[])
{
    FILE *fp, *temp;
    fp = fopen(file, "rb");
    if (fp == NULL)
    {
        perror("Error");
        return;
    }

    temp = fopen("temp.mp3", "wb");
    if (temp == NULL)
    {
        perror("ERROR");
        fclose(fp);
        return;
    }

    char new_frame_id[5];

    switch (option[1])
    {
    case 't':
        strcpy(new_frame_id, "TIT2");
        break;
    case 'a':
        strcpy(new_frame_id, "TPE1");
        break;
    case 'A':
        strcpy(new_frame_id, "TALB");
        break;
    case 'y':
        strcpy(new_frame_id, "TYER");
        break;
    case 'm':
        strcpy(new_frame_id, "TCON");
        break;
    case 'c':
        strcpy(new_frame_id, "COMM");
        break;
    default:
        printf("Invalid edit option\n");
        fclose(fp);
        fclose(temp);
        return;
    }

    char file_head[4];
    if (fread(file_head, sizeof(char), 3, fp) != 3)
    {
        printf("Unable to read file header\n");
        fclose(fp);
        fclose(temp);
        return;
    }
    file_head[3] = '\0';

    if ((strcmp(file_head, "ID3")) != 0)
    {
        printf("Invalid ID3 tag\n");
        fclose(fp);
        fclose(temp);
        return;
    }
    fwrite(file_head, sizeof(char), 3, temp);

    unsigned char file_version[3];
    if (fread(file_version, sizeof(char), 2, fp) != 2)
    {
        printf("Unable to read file version\n");
        fclose(fp);
        fclose(temp);
        return;
    }
    file_version[2] = '\0';

    if (file_version[0] != 3 || file_version[1] != 0)
    {
        printf("Invalid file version\n");
        fclose(fp);
        fclose(temp);
        return;
    }
    fwrite(file_version, sizeof(char), 2, temp);

    unsigned char flags;
    if (fread(&flags, 1, 1, fp) != 1)
    {
        printf("Unable to read flags\n");
        fclose(fp);
        fclose(temp);
        return;
    }
    fwrite(&flags, 1, 1, temp);

    unsigned char header_size[4];
    if (fread(header_size, sizeof(char), 4, fp) != 4)
    {
        printf("Unable to read file ID3 header size\n");
        fclose(fp);
        fclose(temp);
        return;
    }
    fwrite(header_size, sizeof(char), 4, temp);

    int tag_size = 0;
    int i;
    for (i = 0; i < 4; i++)
    {
        tag_size = (tag_size << 8) | header_size[i];
    }

    i = 0;
    int found = 0;
    int new_tag_size = tag_size;
    while (i < tag_size)
    {
        char frame_id[5];
        if (fread(frame_id, sizeof(char), 4, fp) != 4)
        {
            printf("Unable to read frame ID\n");
            fclose(fp);
            fclose(temp);
            return;
        }
        frame_id[4] = '\0';

        if (frame_id[0] == '\0')
        {
            break;
        }

        unsigned char frame_id_size[4];
        if (fread(frame_id_size, sizeof(char), 4, fp) != 4)
        {
            printf("Unable to read frame size\n");
            fclose(fp);
            fclose(temp);
            return;
        }

        int framesize = 0;
        int j;
        for (j = 0; j < 4; j++)
        {
            framesize = (framesize << 8) | frame_id_size[j];
        }

        unsigned char frame_flag[2];
        if (fread(frame_flag, sizeof(char), 2, fp) != 2)
        {
            printf("Unable to read frame flag\n");
            fclose(fp);
            fclose(temp);
            return;
        }

        char *data = malloc(framesize + 1);
        if (data == NULL)
        {
            perror("ERROR");
            fclose(fp);
            fclose(temp);
            return;
        }

        if (fread(data, sizeof(char), framesize, fp) != framesize)
        {
            printf("Unable to read frame data\n");
            free(data);
            fclose(fp);
            fclose(temp);
            return;
        }
        data[framesize] = '\0';

        if (strcmp(frame_id, new_frame_id) == 0)
        {
            found = 1;
            int new_size;
            if (strcmp(frame_id, "COMM") == 0)
            {
                new_size = strlen(value) + 5;
            }
            else
            {
                new_size = strlen(value) + 1;
            }
            new_tag_size = new_tag_size - framesize + new_size;

            unsigned char new_frame_size[4];

            new_frame_size[0] = (new_size >> 24) & 0xFF;
            new_frame_size[1] = (new_size >> 16) & 0xFF;
            new_frame_size[2] = (new_size >> 8) & 0xFF;
            new_frame_size[3] = new_size & 0xFF;

            unsigned char *new_data = malloc(new_size);
            if (new_data == NULL)
            {
                perror("ERROR");
                free(data);
                fclose(fp);
                fclose(temp);
                return;
            }

            if (strcmp(frame_id, "COMM") == 0)
            {
                new_data[0] = 0;
                new_data[1] = 'e';
                new_data[2] = 'n';
                new_data[3] = 'g';
                new_data[4] = '\0';
                strcpy((char *)new_data + 5, value);
            }
            else
            {
                new_data[0] = 0;
                strcpy((char *)new_data + 1, value);
            }

            fwrite(frame_id, 1, 4, temp);
            fwrite(new_frame_size, 1, 4, temp);
            fwrite(frame_flag, 1, 2, temp);
            fwrite(new_data, 1, new_size, temp);

            free(new_data);
        }
        else
        {
            fwrite(frame_id, 1, 4, temp);
            fwrite(frame_id_size, 1, 4, temp);
            fwrite(frame_flag, 1, 2, temp);
            fwrite(data, 1, framesize, temp);
        }

        free(data);
        i += 10 + framesize;
    }

    if (!found)
    {
        printf("Frame not found\n");
        fclose(fp);
        fclose(temp);
        remove("temp.mp3");
        return;
    }

    unsigned char updated_size[4];
    updated_size[0] = (new_tag_size >> 24) & 0xFF;
    updated_size[1] = (new_tag_size >> 16) & 0xFF;
    updated_size[2] = (new_tag_size >> 8) & 0xFF;
    updated_size[3] = new_tag_size & 0xFF;

    char buffer[1024];
    size_t bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0)
    {
        if (fwrite(buffer, 1, bytes, temp) != bytes)
        {
            printf("Unable to write audio data\n");
            fclose(fp);
            fclose(temp);
            return;
        }
    }

    fseek(temp, 6, SEEK_SET);
    fwrite(updated_size, 1, 4, temp);

    fclose(fp);
    fclose(temp);

    if (rename("temp.mp3", file) != 0)
    {
        printf("Rename failed\n");
        return;
    }

    printf("MP3 tag updated successfully!\n");
}