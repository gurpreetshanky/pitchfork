#include <pf/existing/glob_sources.hpp>

#include <set>
#include <stdexcept>

#include <catch2/catch.hpp>

namespace fs = pf::fs;

namespace {
#if STD_FS_IS_EXPERIMENTAL
fs::path relative(fs::path const& path, fs::path const& base) {
    auto canonical_path = fs::canonical(path).string();
    auto canonical_base = fs::canonical(base).string();

    // if (!canonical_path.starts_with(canonical_base))
    if (canonical_path.rfind(canonical_base) != 0) {
        throw std::runtime_error{"path is not relative to supplied base"};
    }

    // + 1 to remove the `/`
    return fs::path{canonical_path.substr(canonical_base.size() + 1)};
}
#else
fs::path relative(fs::path const& path, fs::path const& base) { return fs::relative(path, base); }
#endif

std::vector<fs::path> make_relative(std::vector<fs::path> paths) {
    auto base = fs::path{PF_TEST_BINDIR} / fs::path{"existing/sample/project"};

    for (auto& path : paths) {
        path = ::relative(path, base);
    }

    return paths;
}

template <template <typename...> typename Cont, typename Range>
auto to(Range const& range) {
    return Cont<std::decay_t<decltype(*range.begin())>>(range.begin(), range.end());
}
}  // namespace

TEST_CASE("glob sources") {
    auto expected = std::set<fs::path>{
        fs::path{"src/project/subfolder/source1.c"},
        fs::path{"src/project/subfolder/source2.cc"},
        fs::path{"src/project/subfolder/source3.cpp"},
        fs::path{"src/project/subfolder/source4.cxx"},
        fs::path{"src/project/subfolder/source5.c++"},
        fs::path{"src/project/source1.c"},
        fs::path{"src/project/source2.cc"},
        fs::path{"src/project/source3.cpp"},
        fs::path{"src/project/source4.cxx"},
        fs::path{"src/project/source5.c++"},
    };

    CHECK(::to<std::set>(::make_relative(
              pf::glob_sources(fs::path{PF_TEST_BINDIR} / fs::path{"existing/sample/project"})))
          == expected);
}

TEST_CASE("glob headers") {
    auto expected = std::set<fs::path>{
        fs::path{"src/project/subfolder/header1.h"},
        fs::path{"src/project/subfolder/header2.hh"},
        fs::path{"src/project/subfolder/header3.hpp"},
        fs::path{"src/project/subfolder/header4.hxx"},
        fs::path{"src/project/subfolder/header5.h++"},
        fs::path{"src/project/header1.h"},
        fs::path{"src/project/header2.hh"},
        fs::path{"src/project/header3.hpp"},
        fs::path{"src/project/header4.hxx"},
        fs::path{"src/project/header5.h++"},
    };

    CHECK(::to<std::set>(::make_relative(
              pf::glob_headers(fs::path{PF_TEST_BINDIR} / fs::path{"existing/sample/project"})))
          == expected);
}
