#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <utility>
#include <vector>

#include <boost/program_options.hpp>

#include <clang/AST/Decl.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Frontend/ASTUnit.h>
#include <clang/Lex/Lexer.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Tooling/Core/Replacement.h>
#include <clang/Tooling/JSONCompilationDatabase.h>
#include <clang/Tooling/Tooling.h>

#include <range/v3/all.hpp>

size_t getRandom(size_t max) {
    std::random_device generator;
    return std::uniform_int_distribution<size_t>(0, max - 1)(generator);
}

template <typename Iterator>
Iterator getRandom(Iterator begin, Iterator end) {
    std::advance(begin, getRandom(std::distance(begin, end)));
    return begin;
}

std::vector<const clang::FunctionDecl*> getAllFunctionDeclarations(clang::ASTContext& context) {
    using namespace clang::ast_matchers;
    const auto matcher = functionDecl(isExpansionInMainFile(), isDefinition(), unless(isImplicit())).bind("root");
    const auto matches = match(matcher, context);

    std::vector<const clang::FunctionDecl*> declarations;

    for (const auto& match : matches) {
        const auto* function = match.getNodeAs<clang::FunctionDecl>("root");
        declarations.push_back(function);
    }

    return declarations;
}

int main(int argc, char** argv) {
    namespace po = boost::program_options;

    po::options_description required("Required options");
    po::options_description optional("Optional options");

    // clang-format off
    required.add_options()
        ("compile-commands,p", po::value<std::string>()->required(), "path to compile_commands.json")
        ("filename", po::value<std::string>()->required(), "path to the file to mutate");

    optional.add_options()
        ("help", "produce help message");
    // clang-format on

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, po::options_description().add(required).add(optional)), vm);

    if (vm.count("help")) {
        std::cout << required << std::endl;
        std::cout << optional << std::endl;
        return 1;
    }

    try {
        po::notify(vm);
    } catch (const po::error& e) {
        std::cerr << "Error: " << e.what() << std::endl << std::endl;
        std::cerr << required << std::endl;
        return 1;
    }

    std::string error;
    const auto database = clang::tooling::JSONCompilationDatabase::loadFromFile(
        vm.at("compile-commands").as<std::string>(), error, clang::tooling::JSONCommandLineSyntax::AutoDetect);
    if (!database) {
        std::cerr << "Could not load compilation database." << std::endl << error << std::endl;
        return 1;
    }

    clang::tooling::ClangTool tool(*database, {vm.at("filename").as<std::string>()});
    std::vector<std::unique_ptr<clang::ASTUnit>> ASTs;
    if (tool.buildASTs(ASTs) != 0) {
        std::cerr << "Failed to build the AST." << std::endl;
        return 1;
    }

    assert(ASTs.size() == 1);
    auto& sm = ASTs[0]->getSourceManager();
    const auto& lo = ASTs[0]->getLangOpts();

    auto candidates = getAllFunctionDeclarations(ASTs[0]->getASTContext());
    std::shuffle(candidates.begin(), candidates.end(), std::random_device());

    for (const auto& function : candidates) {
        if (function->getNumParams() < 2) {
            continue;
        }

        const auto first = getRandom(function->getNumParams());
        const auto options
            = ranges::views::iota(0u, function->getNumParams()) | ranges::views::filter([&](auto index) {
                  return function->getParamDecl(index)->getType() != function->getParamDecl(first)->getType();
              })
              | ranges::to<std::vector>();

        if (options.empty()) {
            continue;
        }

        const auto second = *getRandom(options.begin(), options.end());

        const auto firstSourceRange
            = clang::CharSourceRange::getTokenRange(function->getParamDecl(first)->getSourceRange());
        const auto secondSourceRange
            = clang::CharSourceRange::getTokenRange(function->getParamDecl(second)->getSourceRange());

        const auto firstSourceText = clang::Lexer::getSourceText(firstSourceRange, sm, lo);
        const auto secondSourceText = clang::Lexer::getSourceText(secondSourceRange, sm, lo);

        clang::tooling::Replacements replacements;
        // const clang::tooling::Replacement(sm, firstSourceRange, secondSourceText, lo);
        // const clang::tooling::Replacement(sm, secondSourceRange, firstSourceText, lo);

        if (auto error = replacements.add(clang::tooling::Replacement(sm, firstSourceRange, secondSourceText, lo))) {
            llvm::errs() << "Error: " << error;
            return 1;
        }

        if (auto error = replacements.add(clang::tooling::Replacement(sm, secondSourceRange, firstSourceText, lo))) {
            llvm::errs() << "Error: " << error;
            return 1;
        }

        clang::Rewriter rewriter(sm, lo);
        if (!clang::tooling::applyAllReplacements(replacements, rewriter)) {
            std::cerr << "Error: could not apply replacements." << std::endl;
            return 1;
        }

        rewriter.overwriteChangedFiles();

        return 0;
    }

    std::cout << "Could not find any suitable candidates." << std::endl;
    return 1;
}
