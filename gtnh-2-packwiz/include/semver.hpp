#pragma once
#include <string>

class semVer {
  protected:
    struct semVerStruct {
      int major;
      int minor;
      int patch;
      std::string prerelease = "";
      bool usingPrerelease;
    };
  private:
    semVerStruct version;
  public:
    semVer(int major, int minor, int patch, std::string prerelease = "");
    semVer(std::string semverString);
    semVer(semVerStruct structured);
    semVer();
    const std::string string() const;
    bool sameMaj(semVer other);
    bool sameMin(semVer other);
    bool samePat(semVer other);
    bool samePre(semVer other);
    const semVerStruct raw() const;
};