#ifndef PROVISIONAL_CRED
#define PROVISIONAL_CRED

#define JWT_LENGTH 255

#include <Arduino.h>
//#include <string.h>

struct WiFiCred
{
    const char *ssid = "ADVAN MF01-7682FE";
    const char *pass = "83938D57";
};

extern WiFiCred wifiCred;

struct RVMCred
{
    const char *rvmid = "4001";
    const char *secretKey = "12345";
    const char *rootCert = rootCACertificate;
    char jwt[255];

    void getJWT(char output[]);
    void setJWT(char input[]);

const char* rootCACertificate = R"(-----BEGIN CERTIFICATE-----
MIIEzjCCAzagAwIBAgIRAMlVceavoBZKUi5DJo/rt8MwDQYJKoZIhvcNAQELBQAw
fzEeMBwGA1UEChMVbWtjZXJ0IGRldmVsb3BtZW50IENBMSowKAYDVQQLDCFnaWRl
b25AZ2lkZW9ucy1lbmRlYXZvdXIgKEdpZGVvbikxMTAvBgNVBAMMKG1rY2VydCBn
aWRlb25AZ2lkZW9ucy1lbmRlYXZvdXIgKEdpZGVvbikwHhcNMjQxMDA4MDkzNTIw
WhcNMzQxMDA4MDkzNTIwWjB/MR4wHAYDVQQKExVta2NlcnQgZGV2ZWxvcG1lbnQg
Q0ExKjAoBgNVBAsMIWdpZGVvbkBnaWRlb25zLWVuZGVhdm91ciAoR2lkZW9uKTEx
MC8GA1UEAwwobWtjZXJ0IGdpZGVvbkBnaWRlb25zLWVuZGVhdm91ciAoR2lkZW9u
KTCCAaIwDQYJKoZIhvcNAQEBBQADggGPADCCAYoCggGBAM36gxv0xuNizKmJthVK
oTz43ixuGrRyrIKHwejrIv81wMI1V9niErRfCgxS3/mdoy3FumnWnlNqfhNL2b0r
tf7B0Xw+YoKxQORwCuERkIa/cAuJurKqvP1oAVj7T5pgDB3N3eG5byvlOs/7Aqf2
q9t9PWRUHgNOJhWuBlcdZsc6SxoQOD1+WFzw9WF4fVRHM6m3NK/UvhLGXCPoSavz
5nW0/H+R/+RTAnXQDwvy0fR1AuhEYQmhygx9NN4++owM+yuLxoAgXtvyn4wd5dQW
wt1+YMf1niowfKouHwLnTSfnOs5t+Ut9HwuTEiAJ0o6IuSdpDUqgyVVzF8QyzLqD
Dlxi/Ro+qDIMlcAS9LVYFJw2ecOwTdpDoByoqx6+iiRllPWtKqAdXE++oqrQUh4y
SZx2piyX2dtHKsZnPYciePp7QZFTUTO82idRy+/+FJ1rbM5tbD81lJimcjcHskqK
C2qv12+Cv/zSge86o8W0gzCg3fx78XoTX3tnr8uR9KSz7wIDAQABo0UwQzAOBgNV
HQ8BAf8EBAMCAgQwEgYDVR0TAQH/BAgwBgEB/wIBADAdBgNVHQ4EFgQUAeMDvNkZ
Niogw4YoEx3jYmvr2iEwDQYJKoZIhvcNAQELBQADggGBADhLUPMUbEdS75CDa6jg
4CVH2Ryzzlhu+ier0ziURGMGFayNeFvQw1VsFXQO3knBfI4SyLuwR83x8uYRT3LD
zC0kLDtA19Ppwhhpuu4uoWVJw5JcHSflNfSzMuPcSSu1fGqzspk3Mw4CLN6MyIAV
+HBmrZ0Mftg5OMdCaO+DLoyuR9TVXy1SR23dpQfnie3eLRoIwsMccyQgycLutLe0
HVBY1JjI2QudqfomItIA7rOocMH5nQg2zFNK/EJHlOVul2z2+/DzTjI6212XUL7l
hZyl/dAdcmMg2X4un9SAP9fEuwFLLHZJ40eWSFma6dCaDBoKJ4kIiVmduIu2rSiu
vzOFBf4nm1Pwf7J1KgbjpQjScs2TaEkS3AQfloLa5alOQDDMcHCpmFRWpZ/XhBJu
GaekOfiKxaJ25cqTpBOGDyKlL85C7HYu+zEro6SFy/0PSKNwqeLkmn/HM7uuEPQi
yujDZDOfLJlMTiqny9/7xT1t0V8NpuyJPB9r2da08m14uA==
-----END CERTIFICATE-----
)";

    // DELETE LATER!!!!!!
    void previewJWT();
};

extern RVMCred rvmCred;

#endif