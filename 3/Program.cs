using System;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;

namespace _3
{
    static class CustomExtensions {
        public static string RegexReplace(this string text, string regex, string replacement) {
            return Regex.Replace(text, regex, replacement);
        }

        public static string[] RegexSplit(this string text, string regex) {
            return Regex.Split(text, regex);
        }
    }

    class Program
    {
        public static void Main(string[] args){
            int n = int.Parse(Console.ReadLine());

            var wordsToCount = File.ReadAllText("pg17013.txt")
                .ToLower()
                .RegexReplace("[^a-zñáéíóúü]+", " ")
                .RegexSplit("\\s+")
                .Where(s => s.Length >= 3)
                .GroupBy(e => e)
                .ToDictionary(e => e.Key, e => e.Count());

            var orderedList = wordsToCount
                .OrderByDescending(e => e.Value)
                .ThenBy(e => e.Key, StringComparer.Ordinal)
                .Select(e => e.Key)
                .ToList();
            
            var wordsToRanking = orderedList
                .Select((word, ranking) => (word, ranking))
                .ToDictionary(e => e.word, e => e.ranking + 1);

            for(int i=1; i<=n; i++) {
                string caseStr = Console.ReadLine();

                if (int.TryParse(caseStr, out int caseNum)) {
                    Console.WriteLine($"Case #{i}: {orderedList[caseNum - 1]} {wordsToCount[orderedList[caseNum - 1]]}");
                } else {
                    Console.WriteLine($"Case #{i}: {wordsToCount[caseStr]} #{wordsToRanking[caseStr]}");
                }

            }
        }
    }
}
