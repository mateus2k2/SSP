"""Shared Python code for the SSP scripts.

  instances        load an instance in any of the four input formats
  reports          parse a mainCpp solution report
  validation       check a report against its instance (every SSP-USPrC rule)
  results          aggregate validated runs into spreadsheet rows
  csvData          pandas loaders for the ';'-separated job/toolset CSVs
  processingTimes  sample processing times from the real job data

Entry-point scripts in the sibling folders put scripts/ on sys.path and
import this package as `ssp`.
"""
