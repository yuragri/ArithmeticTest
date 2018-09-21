using System.Collections.Generic;

namespace ArithmeticWPF
{
    public class HistoryDataContext
    {
        public List<HistoryUnit> HistoryList { get; set; }

        public HistoryDataContext(List<HistoryUnit> list)
        {
            HistoryList = list;
        }
    }
}
